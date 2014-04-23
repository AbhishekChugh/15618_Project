#include "bvh.hpp"
#ifdef ISPC

#include <string.h>
#include "scene/scene.hpp"

using namespace std;
namespace _462 {
    
    /*struct ComparePoints {
        ComparePoints(int d) { dim = d; }
        int dim;
        bool operator()(const PrimitiveInfo &a,
            const PrimitiveInfo &b) const {
                return a.centroid.v[dim] < b.centroid.v[dim];
        }
    };*/
    void swapVals(PrimitiveInfoList& buildData, uint32_t i, uint32_t j)
    {
	swap(buildData.primitiveNumber[i],buildData.primitiveNumber[j]); 
	
	swap(buildData.centroidx[i],buildData.centroidx[j]); 
	swap(buildData.centroidy[i],buildData.centroidy[j]); 
	swap(buildData.centroidz[i],buildData.centroidz[j]); 
	
	swap(buildData.lowCoordx[i],buildData.lowCoordx[j]); 
	swap(buildData.lowCoordy[i],buildData.lowCoordy[j]); 
	swap(buildData.lowCoordz[i],buildData.lowCoordz[j]); 
	
	swap(buildData.highCoordx[i],buildData.highCoordx[j]); 
	swap(buildData.highCoordy[i],buildData.highCoordy[j]); 
	swap(buildData.highCoordz[i],buildData.highCoordz[j]); 
    }
    unsigned int partition(int start, int end, int dim, float mid, PrimitiveInfoList& buildData, PrimitiveInfoList& buildDataBuffer) {
	float *compareDim = buildData.centroidx;
	if(dim == 1)
		compareDim = buildData.centroidy;
	if(dim == 2)
		compareDim = buildData.centroidz;
	uint32_t front = start, back = end-1; 
	while(front<back)
	{
		if(compareDim[front]<mid)
			front++;
		else if(compareDim[back]>=mid)
			back--;
		else
		{
			swapVals(buildData, front,back);
			front++;back--;
		}
	}
	return front;
       /* memcpy(buildDataBuffer.primitiveNumber + start, buildData.primitiveNumber + start, (end - start) * sizeof(uint32_t));
        
        memcpy(buildDataBuffer.centroidx + start, buildData.centroidx + start, (end - start) * sizeof(float));
        memcpy(buildDataBuffer.centroidy + start, buildData.centroidy + start, (end - start) * sizeof(float));
        memcpy(buildDataBuffer.centroidz + start, buildData.centroidz + start, (end - start) * sizeof(float));

        memcpy(buildDataBuffer.lowCoordx + start, buildData.lowCoordx + start, (end - start) * sizeof(float));
        memcpy(buildDataBuffer.lowCoordy + start, buildData.lowCoordy + start, (end - start) * sizeof(float));
        memcpy(buildDataBuffer.lowCoordz + start, buildData.lowCoordz + start, (end - start) * sizeof(float));

        memcpy(buildDataBuffer.highCoordx + start, buildData.highCoordx + start, (end - start) * sizeof(float));
        memcpy(buildDataBuffer.highCoordy + start, buildData.highCoordy + start, (end - start) * sizeof(float));
        memcpy(buildDataBuffer.highCoordz + start, buildData.highCoordz + start, (end - start) * sizeof(float));

        return ispc::partition_ispc(start, end, dim, mid, buildDataBuffer, buildData);*/
		
    }
    
    void initPrimitiveInfoList(const std::vector<Geometry*>& primitives, PrimitiveInfoList& list, bool allocateOnly)
    {
        uint32_t N = primitives.size();
        list.primitiveNumber =  new uint32_t[N];
        list.centroidx       =  new float[N];
        list.centroidy       =  new float[N];
        list.centroidz       =  new float[N];

        list.lowCoordx       =  new float[N];
        list.lowCoordy       =  new float[N];
        list.lowCoordz       =  new float[N];

        list.highCoordx       =  new float[N];
        list.highCoordy       =  new float[N];
        list.highCoordz       =  new float[N];
        list.primCount = N;
        if(!allocateOnly)for(uint32_t i=0; i<N; i++)
        {
            Vector3 centroid = primitives[i]->bb.centroid();
            list.primitiveNumber[i] = i;

            list.centroidx[i] = centroid.x;
            list.centroidy[i] = centroid.y;
            list.centroidz[i] = centroid.z;
            
            
            list.lowCoordx[i] =  primitives[i]->bb.lowCoord.x;
            list.lowCoordy[i] =  primitives[i]->bb.lowCoord.y;
            list.lowCoordz[i] =  primitives[i]->bb.lowCoord.z;
            
            list.highCoordx[i] =  primitives[i]->bb.highCoord.x;
            list.highCoordy[i] =  primitives[i]->bb.highCoord.y;
            list.highCoordz[i] =  primitives[i]->bb.highCoord.z;
        }
    }
    void AddBox(const PrimitiveInfoList& buildData, uint32_t index, BoundingBox & box)
    {
        box.AddBox(buildData,index);
    }
    void toArray(const Vector3& vec, ispc::float3& arr)
    {
        arr.v[0] = vec[0];
        arr.v[1] = vec[1];
        arr.v[2] = vec[2];
    }
    void toVector(const ispc::float3& arr, Vector3& vec)
    {
        vec[0] = arr.v[0];
        vec[1] = arr.v[1];
        vec[2] = arr.v[2];
    }
    void AddBox(const PrimitiveInfoList& buildData, uint32_t start, uint32_t end, BoundingBox & box)
    {
        ispc::float3 low, high;
        toArray(box.lowCoord , low);
        toArray(box.highCoord, high);
        ispc::AddBox(low, high, buildData, start,end);
        toVector(low,box.lowCoord);
        toVector(high,box.highCoord);
    }
    void AddCentroid(const PrimitiveInfoList& buildData, uint32_t start, uint32_t end, BoundingBox & box)
    {
        ispc::float3 low, high;
        toArray(box.lowCoord , low);
        toArray(box.highCoord, high);
        ispc::AddCentroid(low, high, buildData, start,end);
        toVector(low,box.lowCoord);
        toVector(high,box.highCoord);
    }
    template<class T>
    static void reorder(T* arr, int* order, int size)
    {
        T* temp = new T[size];
        for(int i=0;i<size;i++)
            temp[i] = arr[ order[i] ];
        for(int i=0;i<size;i++)
            arr[i] = temp[i];

        delete[] temp;
    }

    uint32_t SplitEqually(PrimitiveInfoList& buildData, uint32_t start, uint32_t end, uint32_t dim)
    {
        uint32_t mid = (start + end) / 2;
        vector<std::pair<float,int> > vp;
        
        float* compareDim = buildData.centroidx;
        if(dim==1)
            compareDim = buildData.centroidy;
        if(dim==2)
            compareDim = buildData.centroidz;

        for(uint32_t i = start; i<end; i++)
            vp.push_back( make_pair(compareDim[i], i-start));
        int size = end-start;
        sort(vp.begin(), vp.end());

        int* order = new int[size];
        for(int i=0;i<size;i++)
            order[i] = vp[i].second;
        
        reorder(buildData.primitiveNumber + start, order, size);

        reorder(buildData.centroidx + start, order, size);
        reorder(buildData.centroidy + start, order, size);
        reorder(buildData.centroidz + start, order, size);

        reorder(buildData.lowCoordx + start, order, size);
        reorder(buildData.lowCoordy + start, order, size);
        reorder(buildData.lowCoordz + start, order, size);

        reorder(buildData.highCoordx + start, order, size);
        reorder(buildData.highCoordy + start, order, size);
        reorder(buildData.highCoordz + start, order, size);

        delete[] order;
        return mid;
    }
    void clearList(PrimitiveInfoList& list)
    { 
        delete list.primitiveNumber;
        delete list.centroidx;       
        delete list.centroidy;       
        delete list.centroidz;       
                             
        delete list.lowCoordx;       
        delete list.lowCoordy;       
        delete list.lowCoordz;       
         
        delete list.highCoordx;
        delete list.highCoordy;
        delete list.highCoordz;
        list.primCount = 0;
    }
    
    float getCentroidDim(const PrimitiveInfoList& buildData, int index, int dim)
    {
        if(dim==0) return buildData.centroidx[index];
        if(dim==1) return buildData.centroidy[index];
        if(dim==2) return buildData.centroidz[index];
        assert(false);
    }
}/* _462 */

#endif
