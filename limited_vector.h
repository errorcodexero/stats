#ifndef LIMITED_VECTOR_H
#define LIMITED_VECTOR_H

template<typename T,unsigned MAX_LEN>
class Limited_vector{
	unsigned short size_;

	//this isn't just a plain array in case 'T' isn't default constructable.
	//You better hope that your type doesn't have any sort of alignment restictions, because we're packing them in tight!
	char data[MAX_LEN*sizeof(T)];

	T *at(unsigned i){
		return (T*)(data+sizeof(T)*i);
	}

	const T *at(unsigned i)const{
		return (const T*)(data+sizeof(T)*i);
	}

	public:
	Limited_vector():size_(0){}

	Limited_vector(Limited_vector const& l):size_(0){
		for(auto a:l) push_back(a);
	}

	Limited_vector& operator=(Limited_vector const&)nyi

	Limited_vector& operator=(std::vector<T> const& v){
		clear();
		for(auto a:v) push_back(a);
		assert(size_==v.size());
	}

	~Limited_vector(){
		for(unsigned i=0;i<size_;i++){
			at(i)->~T();
		}
	}

	void clear(){
		for(unsigned i=0;i<size_;i++){
			nyi
		}
	}

	void push_back(T t){
		assert(size_<MAX_LEN);
		new (at(size_)) T(t);
		size_++;
	}

	unsigned size()const{ return size_; }

	T const& operator[](unsigned i)const{
		assert(i<size_);
		return *at(i);
	}

	T& operator[](unsigned i){
		assert(i<size_);
		return *at(i);
	}

	T* begin(){ return at(0); }
	T* end(){ return at(size_); }
	const T* begin()const{ return at(0); }
	const T* end()const{ return at(size_); }
};

template<typename T,unsigned MAX_LEN>
bool operator==(std::vector<T>,Limited_vector<T,MAX_LEN>)nyi

template<typename T,unsigned MAX_LEN>
bool operator==(Limited_vector<T,MAX_LEN> a,std::vector<T> b){
	if(a.size()!=b.size()) return 0;
	for(unsigned i=0;i<a.size();i++){
		if(a[i]!=b[i]) return 0;
	}
	return 1;
}

template<typename T,unsigned MAX_LEN>
std::ostream& operator<<(std::ostream& o,Limited_vector<T,MAX_LEN> const& a){
	std::vector<T> v;
	for(auto elem:a) v|=elem;
	return o<<v;
}

#endif