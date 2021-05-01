# WolfGPU
I took the same old school project again to test new things. Wolf3D is relatively simple and I understand the code inside out so it's perfect to test new stuff.</br>
Anyway, the main meat of this project it to learn more about GPU computing via OpenCL. Also since this is not part of any school project, I'll take some own liberties and experiment on some new things.</br>

# TODO
Name may be a bit misleading but it's what this started with but this will now work as more general playground for other stuff as well.</br>
1. GPU rendering - duh! It's there and works very well. There's some things related to it I want to work out. For example my screen height is 1080 and currently I need to allocate 2048 "GPU threads" (with lack or proper term) so that's almost 1000 more than I need and costs me some frames per second. The solution would be joblist but implementation is mystery ATM.
2. CPU rendering - I nuked the old one and I'd like to make a new one that resembles the function GPU runs and a toggle that could be used to demostrate the absolute massive difference of speed the GPU made to rendering images.
3. SIMD - CPU, threads and 'same instruction multiple data'. Again, to compare against the school CPU-only version but this is to pull as much out of CPU without using any GPU. Little bit tempted to do an ASM implementation.
