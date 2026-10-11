{
  description = "Cosmeron reproducible C11 checks and real NixOS VM";
  inputs.nixpkgs.url = "github:nixos/nixpkgs/nixos-unstable";
  outputs = { self, nixpkgs }:
    let
      system = "x86_64-linux";
      pkgs = import nixpkgs { inherit system; };
      portable = pkgs.stdenv.mkDerivation {
        pname = "cosmeron-portable-tests";
        version = "0.1";
        src = self;
        dontConfigure = true;
        buildPhase = ''
          runHook preBuild
          CC=cc sh .github/scripts/test-portable-core.sh
          runHook postBuild
        '';
        installPhase = ''
          runHook preInstall
          mkdir -p "$out/bin"
          cp .build-portable/* "$out/bin/"
          cat > "$out/bin/cosmeron-tests" <<'EOF'
#!/bin/sh
set -eu
bin=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
for test in "$bin"/*; do
  [ "$test" = "$bin/cosmeron-tests" ] && continue
  "$test"
done
EOF
          chmod +x "$out/bin/cosmeron-tests"
          runHook postInstall
        '';
      };
      native = pkgs.stdenv.mkDerivation {
        pname = "cosmeron-posix-regression-tests";
        version = "0.1";
        src = self;
        dontConfigure = true;
        buildPhase = ''
          runHook preBuild
          CC=cc sh .github/scripts/test-bsd-native.sh
          runHook postBuild
        '';
        installPhase = ''
          runHook preInstall
          mkdir -p "$out/bin"
          for module in File_Path Network_Smoke Network_Resolve Concurrency_Atomic; do
            cp ".build-bsd-native/$module" "$out/bin/$module"
          done
          cat > "$out/bin/cosmeron-posix-tests" <<'EOF'
#!/bin/sh
set -eu
base=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
for test in File_Path Network_Smoke Network_Resolve Concurrency_Atomic; do
  "$base/$test"
done
EOF
          chmod +x "$out/bin/cosmeron-posix-tests"
          runHook postInstall
        '';
      };
      nixosVM = pkgs.testers.runNixOSTest {
        name = "cosmeron-real-nixos";
        nodes.machine = { ... }: {
          environment.systemPackages = [ portable native ];
          virtualisation.memorySize = 1536;
        };
        testScript = ''
          start_all()
          machine.wait_for_unit("multi-user.target")
          machine.succeed("${portable}/bin/cosmeron-tests")
          machine.succeed("mkdir -p /tmp/cosmeron && cd /tmp/cosmeron && ${native}/bin/cosmeron-posix-tests")
        '';
      };
    in {
      packages.${system} = {
        default = portable;
        nixos-vm = nixosVM;
        native = native;
      };
      checks.${system}.portable = portable;
      checks.${system}.native = native;
      devShells.${system}.default = pkgs.mkShell {
        packages = [ pkgs.gcc pkgs.clang pkgs.cmake pkgs.gnumake ];
      };
    };
}
