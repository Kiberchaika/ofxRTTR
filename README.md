# RTTR
cool library for reflection

## RTTR build

### osx 

```
cmake CMakeLists.txt
make 
make install
```

### emscripten 

only with *sdk-1.38.9-64bit*

```
emconfigure cmake .    
emmake make clean 
emcc --clear-cache 
emmake make 
```