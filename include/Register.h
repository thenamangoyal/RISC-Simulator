/*Copyright (C) 2016, IIT Ropar
 * This file is part of SimpleRISC simulator.
 *
 * SimpleRISC simulator is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * SimpleRISC simulator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors: Naman Goyal (email: 2015csb1021@iitrpr.ac.in)
 */

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
	in = 0;
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