wx_path=${1}/wxWidgets
boost_path=${1}/boost

clang++ -g -std=c++20 -I $wx_path -I $boost_path -I include `$wx_path/build-macOS/wx-config --cxxflags` -o Syncer src/*.cpp `$wx_path/build-macOS/wx-config --libs`
cp Syncer Syncer.app/Contents/MacOS