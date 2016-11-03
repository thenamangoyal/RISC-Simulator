#ifndef REGISTER_H
#define REGISTER_H

template <typename T>
class Register{
private:
	bool pipeline;
	T in;
	T out;
public:
	Register(bool pipe);
	const T& Read();
	void Write(const T& x);
	void clock();
};

template <typename T>
Register<T>::Register(bool pipe){
	out = 0;
	pipeline = pipe;
}

template <typename T>
const T& Register<T>::Read(){
	if (!pipeline){
		return in;
	}
	else {
		return out;
	}
}

template <typename T>
void Register<T>::Write(const T& x){
	in = x;
}

template <typename T>
void Register<T>::clock(){
	out = in;
}


#endif