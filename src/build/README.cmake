Notes for building Seer with cmake.

% cd seer/src/build
% cmake -DCMAKE_BUILD_TYPE=Debug ..     # Debug release -g.
% cmake -DCMAKE_BUILD_TYPE=Release ..   # Optimized release -O.
% cmake -DCMAKE_CXX_FLAGS=-Wall ..      # With all compile warnings turned on.


% make clean                            # Clean files.
% make seergdb                          # Build Seer.
% sudo make install                     # Install it.

% cmake --build . --config Release
% sudo cmake --install .


