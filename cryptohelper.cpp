#include "cryptohelper.h"


cryptohelper::cryptohelper()
{
}


cryptohelper::~cryptohelper()
{
}
std::string cryptohelper::base64_encode(std::string source) {
	string encoded;
	if (source == "")return "";
	Base64::Encode(source, &encoded);
	return encoded;
}
std::string cryptohelper::base64_decode(std::string source) {
	string decoded;
	if (source == "")return "";
	//MessageBox(NULL, _T("提示信息"), _T("提示信息3"), MB_OK);
	Base64::Decode(source, &decoded);
	//MessageBox(NULL, _T("提示信息"), _T("提示信息4"), MB_OK);
	return decoded;
}
string cryptohelper::rot13(std::string source) {
	string out;
	for (string::iterator it = source.begin(); it != source.end(); it++) {
		if (*it >= 'a'&&*it <= 'z')
			out += (*it - 'a' + 13) % 26 + 'a';
		else if (*it >= 'A'&&*it <= 'Z')
			out += (*it - 'A' + 13) % 26 + 'A';
		else out += *it;
	}
	return out;
}

