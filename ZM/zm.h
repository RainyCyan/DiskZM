#include "z.h"
#include"datautils.h"
#include<string>
#include<cstring>

template <class T,size_t dim>
class ZM{
private:
    std::string index_name;//索引名
    points_t<T,dim> points;//数据点集合
    std::vector<uint64_t> mapped_keys;

    
public:
    ZM(){}

    //mapping function
    void mapping(points_t<T,dim> &raw_points,std::vector<uint64_t> &keys);
    //build function
    // void build();
};

//keys用来存储映射and排序之后的映射值;raw_points需要根据映射后的keys排序
template <class T,size_t dim>
void ZM<T,dim>::mapping(points_t<T,dim> &raw_points,std::vector<uint64_t> &keys){
    size_t N=raw_points.size();
    size_t bit_num=ceil((log(N))/log(2))*2;
    for(size_t i=0;i<N;i++){
        int p[dim];
        for(int j=0;j<dim;j++){
            p[j]=(int) (raw_points[i][j]);
        }
        uint64_t mapped_key=compute_Z_value(p,dim,bit_num);
        keys.push_back(mapped_key);
    }
    //sort by the mapped_z_value
}