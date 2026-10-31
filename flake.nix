{
  description = "Rien Prescript Beeper firmware toolchain";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    arduino-nix.url = "github:bouk/arduino-nix";
    arduino-indexes = {
      url = "github:bouk/arduino-indexes";
      flake = false;
    };
  };

  outputs =
    {
      self,
      nixpkgs,
      arduino-nix,
      arduino-indexes,
    }:
    let
      systems = [
        "x86_64-linux"
        "aarch64-linux"
      ];
      overlays = [
        arduino-nix.overlay
        (arduino-nix.mkArduinoPackageOverlay "${arduino-indexes}/index/package_index.json")
        (arduino-nix.mkArduinoPackageOverlay "${arduino-indexes}/index/package_esp32_index.json")
        (arduino-nix.mkArduinoLibraryOverlay "${arduino-indexes}/index/library_index.json")
      ];
      forAllSystems =
        f: nixpkgs.lib.genAttrs systems (system: f (import nixpkgs { inherit system overlays; }));

      esp32CoreVersion = "3.3.12";
      gfxVersion = "1.6.9";
      fqbn = "esp32:esp32:lilygo_t_display_s3";
      sketchDir = "firmware/T-Display-S3/PrescriptBeeper";
    in
    {
      packages = forAllSystems (pkgs: rec {
        arduino-cli = pkgs.wrapArduinoCLI {
          packages = [ pkgs.arduinoPackages.platforms.esp32.esp32.${esp32CoreVersion} ];
          libraries = [ pkgs.arduinoLibraries."GFX Library for Arduino".${gfxVersion} ];
        };

        build = pkgs.writeShellApplication {
          name = "beeper-build";
          runtimeInputs = [
            arduino-cli
            pkgs.git
          ];
          text = ''
            cd "$(git rev-parse --show-toplevel)/${sketchDir}"
            arduino-cli compile --fqbn ${fqbn} --build-property build.defines=-DBOARD_HAS_PSRAM --output-dir build "$@" .
            chmod -R u+w build
          '';
        };

        flash = pkgs.writeShellApplication {
          name = "beeper-flash";
          runtimeInputs = [
            arduino-cli
            pkgs.git
          ];
          text = ''
            cd "$(git rev-parse --show-toplevel)/${sketchDir}"
            port="''${1:-/dev/ttyACM0}"
            arduino-cli upload --fqbn ${fqbn} --input-dir build -p "$port"
          '';
        };

        monitor = pkgs.writeShellApplication {
          name = "beeper-monitor";
          runtimeInputs = [ arduino-cli ];
          text = ''
            port="''${1:-/dev/ttyACM0}"
            arduino-cli monitor -p "$port" --config baudrate=115200
          '';
        };

        default = build;
      });

      devShells = forAllSystems (pkgs: {
        default = pkgs.mkShell {
          packages = with self.packages.${pkgs.stdenv.hostPlatform.system}; [
            arduino-cli
            build
            flash
            monitor
            pkgs.esptool
          ];
        };
      });

      formatter = forAllSystems (pkgs: pkgs.nixfmt-rs);
    };
}
