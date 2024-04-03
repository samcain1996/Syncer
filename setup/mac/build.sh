wx_path=${1}/wxWidgets
boost_path=${1}/boost

clang++ -std=c++20 -I $wx_path -I $boost_path -I include -I include/GUI `$wx_path/build-macOS/wx-config --cxxflags` -o Syncer src/*.cpp src/GUI/*.cpp `$wx_path/build-macOS/wx-config --libs`
mv Syncer Syncer.app/Contents/MacOS