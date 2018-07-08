/*
 * comm.cpp
 *
 *  Created on: Feb 28, 2018
 *      Author: dipsy
 */

#include "comm.h"

uint8_t Comm::CalChecksum(Byte* buf,int length){
	uint8_t sum = 0;
	for(int i=0; i<length; i++){
		if(i == length-2)continue;
		sum+=buf[i];
	}
	return sum;
}

uint8_t Comm::CalChecksum(const vector<Byte>& buf){
	uint8_t sum = 0, length = buf.size();
	for(int i=0; i<length; i++){
		if(i == length-2)continue;
		sum+=buf[i];
	}
	return sum;
}

uint8_t Comm::QueuePackage(Package pkg){
	pkg.id = ++historic_package_sum;
	m_sendqueue.push_back(pkg);
	SendPackageImmediate(pkg);
	return pkg.id;
}

void Comm::SendPackageImmediate(const Package& pkg){
	sent_package_count++;
	const size_t len = 6 + pkg.data.size();
	Byte* buf = new Byte[len];
	Byte debug[100];
	memset(debug,0,100);
	memset(buf,0,len);
	buf[0] = 0xAA;
	buf[1] = len;
	buf[2] = pkg.type;
	buf[3] = pkg.id;
	memcpy(buf+4,&pkg.data[0],pkg.data.size());
	buf[len-1] = 0xFF;
	buf[len-2] = CalChecksum(buf,len);
	memcpy(debug,buf,len);
	SendBuffer(buf,len);
	delete buf;
}

void Comm::Period(){
	for(const Package& pkg:m_sendqueue){
		SendPackageImmediate(pkg);
	}
}

bool Comm::Listener(const Byte* data, const size_t& size){
	bytecount++;
	if(buffer.size()>100){
		while(1);
	}
	memset(debug,0,100);
	memcpy(debug,&*buffer.begin(),buffer.size());
	for(uint8_t i=0; i<size; i++){
		const char& ch = data[i];
		if (!pkg_start && ch == 0xAA) {
			buffer.push_back(ch);
			pkg_start = true;
			len = 0;
		} else if (pkg_start && len == 0) {
			len = ch;
			buffer.push_back(ch);
			if(len>=100){	//wrong package length
				pkg_start = false;
			}
		} else if (pkg_start && buffer.size() < len - 1) {
			buffer.push_back(ch);
		} else if (pkg_start && buffer.size() == len - 1 && ch == 0xFF) {
			buffer.push_back(ch);
			BuildBufferPackage(buffer);
			pkg_start = false;
			len = 0;
			buffer.clear();
		} else {
			pkg_start = false;
			len = 0;
			buffer.clear();
			destroyed_count++;
		}
	}
	return true;
}

void Comm::BuildBufferPackage(const vector<Byte>& buffer){
	++handled_package_sum;
	memset(debug,0,100);
	memcpy(debug,&*buffer.begin(),buffer[1]);

	uint8_t checksum = 0;
	for(int i=0;i<buffer.size();i++){
		if(i==buffer.size()-2)continue;
		checksum += buffer[i];
	}
	if(checksum!=CalChecksum(buffer)){
		return;
	}
	Package pkg = {buffer[2],buffer[3],{}};
	for(int i=4; i<buffer.size()-2;i++){
		pkg.data.push_back(buffer[i]);
	}
	Handler(pkg);
}

void Comm::Handler(const Package& pkg){
	if(pkg.type == PkgType::kACK){
		RemoveQueuedPackage(pkg.id);
	} else {
		SendPackageImmediate({PkgType::kACK,pkg.id,{}});
	}
	if(m_CustomHandler){
		m_CustomHandler(pkg);
	}
}

bool Comm::RemoveQueuedPackage(uint8_t id){
	for(uint32_t i=0; i<m_sendqueue.size(); i++){
		if(m_sendqueue[i].id == id){
			m_sendqueue.erase(m_sendqueue.begin()+i);
			return true;
		}
	}
	return false;
}
