git pull origin main
cd externals
cd NekoLib
git checkout main
git pull origin main
git submodule update --init --recursive
cd ..
cd NekoPhysics
git checkout main
git pull origin main
cd externals/unordered_dense
git checkout v4.4.0
cd ../../../..
git pull origin main