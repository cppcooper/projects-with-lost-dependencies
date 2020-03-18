# Status
These projects have long lost dependencies, some may not though. Most of the code these are all based on is essentially the same code, but - potentially - at different points in time and thus probably not compatible. Others, word-search that is, seem to have all code in src/. They all appeared to, for the most part, have binaries that probably work when given access to Assets/ and any required binaries. A few appeared to include the libraries in their repos, so I've kept those but moved them and make no guarantees that they are sufficient for building any of the projects.

## hyper-maze
Probably won't build, but will probably run with the executable.

## word-search
Looks like it should build, at cursory glance. The executable should run, I think - might not if data is missing.

## bullet-hell
Might build with the libraries included. The executable should work.

## recursive-path-finder
Might build, but will probably run with any of the four executables. The four executables are different the path finder's different compile time settings, as seen in the code.

## maze-gen
Pretty sure there is no hope of this building, but it probably would not be difficult to remove the dependencies. Also doesn't have an executable, so that's the only way it is running.
