let
  hostPkgs = import <nixpkgs> {};
  nixpkgs = (hostPkgs.fetchFromGitHub {
    owner = "NixOS";
    repo = "nixpkgs-channels";
    rev = "nixos-unstable";
    sha256 = "0im8l87nghsp4z7swidgg2qpx9mxidnc0zs7a86qvw8jh7b6sbv2";
  });
in
  with import nixpkgs {
    overlays = [(self: super:
      {
        python3 = super.python3.override {
          packageOverrides = py-self: py-super: {
            matplotlib = py-super.matplotlib.override {
              enableTk = true;
              enableQt = true;
            };
          };
        };
      }
    )];
  };
  stdenv.mkDerivation {
    name = "PCMSolver-dev";
    buildInputs = [
      boost
      bundler
      ccache
      clang
      clang-analyzer
      clang-tools
      cmake
      doxygen
      exa
      ffmpeg
      flameGraph
      gfortran
      gperftools
      graphviz
      lcov
      pipenv
      python3Full
      python3Packages.docopt
      python3Packages.jupyter
      python3Packages.matplotlib
      python3Packages.numpy
      python3Packages.pyyaml
      python3Packages.yapf
      valgrind
      zlib
    ];
    src = null;
    shellHook = ''
    export NINJA_STATUS="[Built edge %f of %t in %e sec]"
    SOURCE_DATE_EPOCH=$(date +%s)
    source $(pipenv --venv)/bin/activate
    '';
  }
