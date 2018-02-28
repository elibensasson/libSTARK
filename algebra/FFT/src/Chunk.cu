//#include "Chunk.cuh"
//#include <bitset>
//#include <iostream>
//#include <cstdio>
//
//using namespace std;
//namespace FFF{
//__constant__ idx_t p_Mod[max_nonzero_coefs_in_mod];
//__constant__ idx_t p_ModLen;
//__constant__ Element element_mul;
//__constant__ Chunk c;
//
//
//__device__ void a_chunkToNormal(Chunk *d_a, Elements_Chunk *d_b, idx_t idx)
//{
//	cell_t ans = 0;
//	idx_t element_idx = idx & andMask(Chunk::log_elements_in_chunk);
//	idx_t cell_idx = idx >> Chunk::log_elements_in_chunk;
//	for(unsigned int i = cell_idx<<Element::log_bits_in_cell ; i < ((cell_idx+1)<<Element::log_bits_in_cell); ++i)
//		ans^=(((cell_t)(((d_a->v[i])>>(element_idx))&1))<<(i-(cell_idx<<Element::log_bits_in_cell)));
//	d_b->e[element_idx].c[cell_idx]=ans;
//}
//__global__ void k_chunkToNormal(Chunk *d_a,Elements_Chunk *d_b , len_t len)
//{
//	const unsigned int threads_in_chunk = Chunk::elements_in_chunk * Element::element_len;
//	 __shared__ Chunk input[max_block_size / threads_in_chunk];
//	idx_t idx = threadIdx.x + blockDim.x*blockIdx.x;
//	if(idx >= len*threads_in_chunk)
//		return;
//	idx_t chunkIdx = (idx) / (Element::element_len*Chunk::elements_in_chunk);
//	idx_t in_chunkIdx = (idx  & (Element::element_len * Chunk::elements_in_chunk - 1));
//	idx_t chunks_in_block = blockDim.x / Chunk::cells_in_chunk;
//	idx_t inBlockChunkIdx = chunkIdx & (threads_in_chunk-1);
//	for(unsigned int i = 0 ; i < sizeof(cell_t)/sizeof(chunk_cell_t) ; ++i){
//		input[inBlockChunkIdx].v[in_chunkIdx + i*threads_in_chunk] = d_a[chunkIdx].v[in_chunkIdx+i*threads_in_chunk];
//	}
//	a_chunkToNormal(&(input[inBlockChunkIdx]), &(d_b[chunkIdx]),in_chunkIdx);
//}
//__host__	void Chunk::chunkToNormal(Chunk(*h_a), Elements_Chunk(*h_b), len_t len, bool copy)
//{
//	//Declare device variables
//	Chunk (*d_a);
//	Elements_Chunk (*d_b);
//
//	const unsigned int num_element = len*elements_in_chunk;
//	const unsigned int threads = Element::element_len * num_element;
//
//	//Define Block  and Grid Size.
//	dim3 blockSize(max_block_size,1,1);
//	dim3 gridSize(sizeCiel(threads,max_block_size),1,1);
//	if(copy){
//	//Allocate Memory on GPU. (global)
//		cudaMalloc(&d_a,sizeof(Chunk)*len);
//		cudaMalloc(&d_b,sizeof(Elements_Chunk)*len);
//
//	//Copy memory to GPU.
//		cudaMemcpy(d_a,h_a,sizeof(Chunk)*len,cudaMemcpyHostToDevice);
//	} else {
//		d_a = h_a;
//		d_b = h_b;
//	}
//
//	//Launch Kernel
//	k_chunkToNormal<<<gridSize,blockSize>>>(d_a,d_b,len);
//	if(copy){
//	//Copy results back to memory
//		cudaMemcpy(h_b,d_b,sizeof(Elements_Chunk)*len,cudaMemcpyDeviceToHost);
//
//	//Free allocated memory.
//		cudaFree(d_a);
//		cudaFree(d_b);
//	}
//}
//__device__ void a_normalToChunk(Elements_Chunk *d_a, Chunk *d_b, idx_t idx)
//{
//	chunk_cell_t ans = 0;
//	idx_t cell_idx = idx>>Element::log_bits_in_cell;
//	for(unsigned int i = 0 ; i < Chunk::elements_in_chunk ; ++i)
//		ans^=((((d_a->e[i].c[cell_idx])>>(idx& andMask(Element::log_bits_in_cell)))&1)<<i);
//	d_b->v[idx]=ans;
//}
//__global__		void k_normalToChunk(Elements_Chunk *d_a,Chunk *d_b , len_t len)
//{
//	idx_t idx = threadIdx.x + blockDim.x*blockIdx.x;
//	if(idx >= (len<<Chunk::log_cells_in_chunk))
//		return;
//	idx_t chunkIdx = (idx) >> Chunk::log_cells_in_chunk;
//	idx_t in_chunkIdx = (idx  & andMask(Chunk::log_cells_in_chunk));
//	a_normalToChunk(&(d_a[chunkIdx]),&(d_b[chunkIdx]),in_chunkIdx);
//}
//__host__	void Chunk::normalToChunk(Elements_Chunk(*h_a), Chunk (*h_b), len_t len,bool copy)
//{
//
//	//Declare device variables
//	Elements_Chunk (*d_a);
//	Chunk (*d_b);
//
//	const unsigned int threads = len<<Chunk::log_cells_in_chunk;
//
//	//Define Block  and Grid Size.
//	dim3 blockSize(max_block_size,1,1);
//	dim3 gridSize(sizeCiel(threads,max_block_size),1,1);
//
//	//Allocate Memory on GPU. (global)
//	if(copy){
//		cudaMalloc(&d_a,sizeof(Elements_Chunk)*len);
//		cudaMalloc(&d_b,sizeof(Chunk)*len);
//
//	//Copy memory to GPU.
//		cudaMemcpy(d_a,h_a,sizeof(Elements_Chunk)*len,cudaMemcpyHostToDevice);
//	} else{
//		d_a = h_a;
//		d_b = h_b;
//	}
//	//Launch Kernel
//	k_normalToChunk<<<gridSize,blockSize>>>(d_a,d_b,len);
//
//	//Copy results back to memory
//	if(copy){
//		cudaMemcpy(h_b,d_b,sizeof(Chunk)*len,cudaMemcpyDeviceToHost);
//		//Free allocated memory.
//		cudaFree(d_a);
//		cudaFree(d_b);
//	}
//
//}
//__host__ 	void Chunk::setMod(){
//	cudaMemcpyToSymbol(p_Mod,&(Element::irr_poly_index[ord>>log_warp_size]),sizeof(idx_t)*max_nonzero_coefs_in_mod);
//	cudaMemcpyToSymbol(p_ModLen,&(Element::mod_len[ord>>log_warp_size]),sizeof(idx_t));
//}
//__device__ void Chunk::chunk_reduce_xor(Chunk *a, Chunk *c_bottom, Chunk*c_top, idx_t idx)
//{
//	chunk_cell_t ans=c_bottom->v[idx];
//	unsigned int temp_idx;
//	for(idx_t i = 0 ; i < p_ModLen ; ++i)
//	{
//		for(idx_t j = 0 ; j < p_ModLen ; ++j)
//		{
//			temp_idx = idx+(ord<<1)-p_Mod[i]-p_Mod[j];
//			if(temp_idx >= (ord<<1)-p_Mod[j] && temp_idx < (ord<<1))
//				ans^=c_top->v[temp_idx-ord];
//		}
//	}
//	a->v[idx]^=ans;
//}
//__device__ void Chunk::chunk_xor(Chunk *a, Chunk* b, idx_t idx){
//	a->v[idx]^=b->v[idx];
//}
//__device__ void Chunk::chunk_reduce_xor(Chunk *a, Chunk *c_bottom, idx_t idx,Chunk* to_xor ,int shift)
//{
//	unsigned int k = p_ModLen;
//	for(unsigned int i = 0 ; i < (ord>>1); i+=warp_size)
//		for(unsigned int j = 0 ; j+1 < k ; ++j)
//		{
//			c_bottom->v[(ord>>1)+idx+i+p_Mod[j]]^=c_bottom->v[(ord>>1)+ord+idx+i];
//		}
//	for(unsigned int i = 0 ; i < (ord>>1); i+=warp_size)
//		for(unsigned int j = 0 ; (j+1) < k ; ++j)
//		{
//			c_bottom->v[idx+i+p_Mod[j]]^=c_bottom->v[ord+idx+i];
//		}
//		for(unsigned int i = 0 ; i < ord ; i+=warp_size){
//			to_xor->v[idx+i]^=(c_bottom->v[idx+i]>>shift);
//		}
//}
//
//__device__ void Chunk::chunk_reduce(Chunk *a, Chunk *c_bottom, idx_t idx)
//{
//	unsigned int k = p_ModLen;
//	for(unsigned int i = 0 ; i < (ord>>1); i+=warp_size)
//		for(unsigned int j = 0 ; j+1 < k ; ++j)
//		{
//			c_bottom->v[(ord>>1)+idx+i+p_Mod[j]]^=c_bottom->v[(ord>>1)+ord+idx+i];
//		}
//	for(unsigned int i = 0 ; i < (ord>>1); i+=warp_size)
//		for(unsigned int j = 0 ; (j+1) < k ; ++j)
//		{
//			c_bottom->v[idx+i+p_Mod[j]]^=c_bottom->v[ord+idx+i];
//		}
//		for(unsigned int i = 0 ; i < ord ; i+=warp_size){
//			a->v[idx+i]=c_bottom->v[idx+i];
//		}
//}
//__device__ void Chunk::chunkClmul(Chunk (*a),  Element (*e), idx_t idx, Chunk (*c))
//{
//	chunk_cell_t my_ans[2][(ord>>(log_warp_size))]={0};
//	for(unsigned int k = 0 ; k < ord ; ++k)
//	{
//		if(EXTRACT_BIT(e->c,k))
//			for(unsigned int t = 0 ; t <  (ord>>log_warp_size); ++t)
//			{
//				int b = (k>(idx+warp_size*t));
//				my_ans[b][t]^=a->v[idx+warp_size*t+(b<<log_ord)-k];
//			}
//	}
//	for(unsigned int i = 0 ; i < (ord>>log_warp_size); ++i)
//	{
//		c->v[idx+i*warp_size] = my_ans[0][i];
//		c->v[ord+idx+i*warp_size] = my_ans[1][i];
//	}
//}
//__device__ void Chunk::aux_k_clmul(Chunk *a, Element* e, len_t len,Chunk* c_shared)
//{
//
//	idx_t idx = blockDim.x*blockIdx.x+threadIdx.x;
//	const idx_t chunk_idx = (idx >> Chunk::log_threads_in_chunk);
//	const idx_t in_chunk_idx = idx & andMask(Chunk::log_threads_in_chunk);
//	const idx_t shared_chunk_idx = ((idx & andMask(log_max_block_size)) >> (Chunk::log_threads_in_chunk));
//	Chunk* my_shared_chunk = c_shared+(shared_chunk_idx<<1);
//	for(unsigned int i =0 ; i < Chunk::ord ;i+=warp_size)
//		my_shared_chunk->v[in_chunk_idx+i]=a[chunk_idx].v[in_chunk_idx+i];
//	Chunk::chunkClmul(my_shared_chunk,e,in_chunk_idx,my_shared_chunk);
//	Chunk::chunk_reduce(a+chunk_idx,my_shared_chunk,in_chunk_idx);
//}
//__global__ void k_clmul(Chunk *a,Element *e,len_t len )
//{
//	const idx_t shared_len = max_block_size>>Chunk::log_threads_in_chunk;
//	__shared__ Chunk c_shared[shared_len<<1];
//	idx_t idx = blockDim.x*blockIdx.x+threadIdx.x;
//	if(idx>=(len<<Chunk::log_threads_in_chunk))
//		return;
//	Chunk::aux_k_clmul(a,e,len,c_shared);
//}
//__host__	void Chunk::mul(Chunk (*h_a),Element (*h_e),len_t len, Chunk (*h_res)){
//
//#ifdef __MEASURE
//	cudaEvent_t start,stop;
//	float time;
//	cudaEventCreate(&start);
//	cudaEventCreate(&stop);
//#endif
//	//Declare device variables
//	Chunk 	(*d_a);
//	Element (*d_e);
//
//	//Define Block  and Grid Size.
//	dim3 blockSize(max_block_size,1,1);
//	dim3 gridSize(sizeCiel(len<<Chunk::log_threads_in_chunk,max_block_size),1,1);
//
//	//Allocate Memory on GPU. (global)
//	cudaMalloc(&d_a,sizeof(Chunk)*len);
//	cudaMalloc(&d_e,sizeof(Element));
//
//	//Copy memory to GPU.
//	cudaMemcpy(d_a,h_a,sizeof(Chunk)*len,cudaMemcpyHostToDevice);
//	cudaMemcpy(d_e,h_e,sizeof(Element),cudaMemcpyHostToDevice);
//
//	//Set Mod
//	setMod();
////	setElementMul(h_e);
//	//Launch Kernel
//#ifdef __MEASURE
//	cudaEventRecord(start,0);
//#endif
//	k_clmul<<<gridSize,blockSize>>>(d_a,d_e,len);
//#ifdef __MEASURE
//	cudaEventRecord(stop,0);
//#endif
//
//	//Copy results to host
//	cudaMemcpy(h_res,d_a,sizeof(Chunk)*len,cudaMemcpyDeviceToHost);
//	//Free allocated memory.
//	cudaFree(d_a);
//	cudaFree(d_e);
//#ifdef __MEASURE
//	cudaEventElapsedTime(&time,start,stop);
//	printf("Time for the mul: %f ms on %d chunks \n",time,len);
//#endif
//}
//
//__global__ void k_add(Chunk (*a), Chunk (*b), len_t l)
//{
//	unsigned int idx = threadIdx.x+blockIdx.x*blockDim.x;
//	if(idx>=l*Chunk::cells_in_chunk)
//		return;
//	((chunk_cell_t*)a)[idx]^=((chunk_cell_t*)b)[idx];
//}
//__host__ void Chunk::add(Chunk (*h_a),Chunk (*h_b),len_t len)
//{
//
//	//Declare device variables
//	Chunk (*d_a);
//	Chunk (*d_b);
//
//	//Define Block  and Grid Size.
//	dim3 blockSize(max_block_size,1,1);
//	dim3 gridSize(sizeCiel(max_block_size,len),1,1);
//
//	//Allocate Memory on GPU. (global)
//	cudaMalloc(&d_a,sizeof(Chunk)*len);
//	cudaMalloc(&d_b,sizeof(Chunk)*len);
//
//	//Copy memory to GPU.
//	cudaMemcpy(d_a,h_a,sizeof(Chunk)*len,cudaMemcpyHostToDevice);
//	cudaMemcpy(d_b,h_b,sizeof(Chunk)*len,cudaMemcpyHostToDevice);
//
//	//Launch Kernel
//	k_add<<<gridSize,blockSize>>>(d_a,d_b,len);
//
//	//Copy results to CPU memory
//	cudaMemcpy(h_a,d_a,sizeof(Chunk)*len,cudaMemcpyDeviceToHost);
//
//	//Free allocated memory.
//	cudaFree(d_a);
//	cudaFree(d_b);
//}
//__host__ void Chunk::print() const {
//	for(unsigned int i = 0 ; i < cells_in_chunk ; ++i){
//		cout << bitset<bits_in_byte*sizeof(chunk_cell_t)>(this->v[i])<<endl;
//	}
//}
//__host__	void Elements_Chunk::print() const{
//		for(unsigned int i = 0 ; i < elements_in_elements_chunk ; ++i){
//			Element::printElement(this->e[i]);
//			cout<<endl;
//		}
//	}
////Mul chunk by another chunk
//__device__ void Chunk::clmul_by_chunk(Chunk& a, Chunk& e, idx_t idx, Chunk* c){
//	chunk_cell_t my_ans[2][(ord>>(log_warp_size))]={0};
//	for(unsigned int k = 0 ; k < ord ; ++k)
//		for(unsigned int t = 0 ; t <  (ord>>log_warp_size); ++t)
//		{
//			int b = (k>(idx+warp_size*t));
//			my_ans[b][t]^=a.v[idx+warp_size*t+(b<<log_ord)-k]&e.v[k];
//		}
//	for(unsigned int i = 0 ; i < (ord>>log_warp_size); ++i)
//	{
//		c->v[idx+i*warp_size] = my_ans[0][i];
//		c->v[ord+idx+i*warp_size] = my_ans[1][i];
//	}
//}
////Mul chunk by another chunk
//__global__ void k_mul_chunk(Chunk* cs, Chunk* c, len_t cs_len)
//{
//	const idx_t shared_len = max_block_size>>Chunk::log_threads_in_chunk;
//	__shared__ Chunk c_shared[shared_len<<1];
//	idx_t idx = blockDim.x*blockIdx.x+threadIdx.x;
//	if(idx>=(cs_len<<Chunk::log_threads_in_chunk))
//		return;
//	const idx_t chunk_idx = (idx >> Chunk::log_threads_in_chunk);
//	const idx_t in_chunk_idx = idx & andMask(Chunk::log_threads_in_chunk);
//	const idx_t shared_chunk_idx = ((idx & andMask(log_max_block_size)) >> (Chunk::log_threads_in_chunk));
//	Chunk* my_shared_chunk = c_shared+(shared_chunk_idx<<1);
//	for(unsigned int i =0 ; i < Chunk::ord ;i+=warp_size){
//		my_shared_chunk->v[in_chunk_idx+i]=cs[chunk_idx].v[in_chunk_idx+i];
//		my_shared_chunk[1].v[in_chunk_idx+i]=c->v[in_chunk_idx+i];
//	}
//	Chunk::clmul_by_chunk(my_shared_chunk[0],my_shared_chunk[1],in_chunk_idx,my_shared_chunk);
//	Chunk::chunk_reduce(cs+chunk_idx,my_shared_chunk,in_chunk_idx);
//}
//__global__ void k_mul_chunk_xor(Chunk* cs, Chunk* c, len_t cs_len,Chunk* to_xor, int shift = 0)
//{
//	const idx_t shared_len = max_block_size>>Chunk::log_threads_in_chunk;
//	__shared__ Chunk c_shared[shared_len<<1];
//	idx_t idx = blockDim.x*blockIdx.x+threadIdx.x;
//	if(idx>=(cs_len<<Chunk::log_threads_in_chunk))
//		return;
//	const idx_t chunk_idx = (idx >> Chunk::log_threads_in_chunk);
//	const idx_t in_chunk_idx = idx & andMask(Chunk::log_threads_in_chunk);
//	const idx_t shared_chunk_idx = ((idx & andMask(log_max_block_size)) >> (Chunk::log_threads_in_chunk));
//	Chunk* my_shared_chunk = c_shared+(shared_chunk_idx<<1);
//	for(unsigned int i =0 ; i < Chunk::ord ;i+=warp_size){
//		my_shared_chunk->v[in_chunk_idx+i]=cs[chunk_idx].v[in_chunk_idx+i];
//		my_shared_chunk[1].v[in_chunk_idx+i]=c->v[in_chunk_idx+i];
//	}
//	Chunk::clmul_by_chunk(my_shared_chunk[0],my_shared_chunk[1],in_chunk_idx,my_shared_chunk);
//	Chunk::chunk_reduce_xor(cs+chunk_idx,my_shared_chunk,in_chunk_idx,to_xor,shift);
//}
////Mul a chunk by a chunk
//void Chunk::chunk_mul(Chunk (* h_a), Chunk (*h_b) , len_t len, Chunk (*h_res), bool copy, bool do_xor, int shift){
//#ifdef __MEASURE
//	cudaEvent_t start,stop;
//	float time;
//	cudaEventCreate(&start);
//	cudaEventCreate(&stop);
//#endif
//	//Declare device variables
//	Chunk 	(*d_a);
//	Chunk   (*d_b);
//
//	//Define Block  and Grid Size.
//	dim3 blockSize(max_block_size,1,1);
//	dim3 gridSize(sizeCiel(len<<Chunk::log_threads_in_chunk,max_block_size),1,1);
//
//	if(copy){
//	//Allocate Memory on GPU. (global)
//		cudaMalloc(&d_a,sizeof(Chunk)*len);
//		cudaMalloc(&d_b,sizeof(Chunk));
//	//Copy memory to GPU.
//		cudaMemcpy(d_a,h_a,sizeof(Chunk)*len,cudaMemcpyHostToDevice);
//		cudaMemcpy(d_b,h_b,sizeof(Chunk),cudaMemcpyHostToDevice);
//	} else {
//		d_a = h_a;
//		d_b = h_b;
//	}
//
//
//	//Set Mod
//	setMod();
////	setElementMul(h_e);
//	//Launch Kernel
//#ifdef __MEASURE
//	cudaEventRecord(start,0);
//#endif
//	if(do_xor)
//		k_mul_chunk_xor<<<gridSize,blockSize>>>(d_a,d_b,len,d_a,shift);
//	else
//		k_mul_chunk<<<gridSize,blockSize>>>(d_a,d_b,len);
//#ifdef __MEASURE
//	cudaEventRecord(stop,0);
//#endif
//
//	if(copy){
//	//Copy results to host
//		cudaMemcpy(h_res,d_a,sizeof(Chunk)*len,cudaMemcpyDeviceToHost);
//	//Free allocated memory.
//		cudaFree(d_a);
//		cudaFree(d_b);
//	}
//#ifdef __MEASURE
//	cudaEventElapsedTime(&time,start,stop);
//	printf("Time for the mul: %f ms on %d chunks \n",time,len);
//#endif
//}
//}
