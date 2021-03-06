#include<iostream>
#include<jsoncpp/json/reader.h>
#include<jsoncpp/json/writer.h>
#include "../util.h"

using namespace std;

void setIndentLength(int){}
void setIndentUseSpace(bool){}
bool getCurrentEncoding(){ return 0; }
bool getCurrentIndentLength(){ return 0; }
bool getCurrentIndentUseSpace(){ return 0; }

int main(){

	Json::Value root; // will contains the root value after parsing.
	Json::Reader reader;
	string config_doc="{\"what\":\"that\"}";
	bool parsingSuccessful = reader.parse( config_doc, root );
	if ( !parsingSuccessful ){
		// report to the user the failure and their locations in the document.
		std::cout << "Failed to parse configuration\n"<< reader.getFormattedErrorMessages();
		return 1;
	}
	// Get the value of the member of root named 'encoding', return 'UTF-8' if there is no
	// such member.
	std::string encoding = root.get("encoding", "UTF-8" ).asString();
	// Get the value of the member of root named 'encoding', return a 'null' value if
	// there is no such member.
	const Json::Value plugins = root["plug-ins"];
	for ( int index = 0; index < plugins.size(); ++index ){
		 // Iterates over the sequence elements.
		cout<<"pl:"<<plugins[index].asString();
	}

	setIndentLength( root["indent"].get("length", 3).asInt() );
	setIndentUseSpace( root["indent"].get("use_space", true).asBool() );
	// ...
	// At application shutdown to make the new configuration document:
	// Since Json::Value has implicit constructor for all value types, it is not
	// necessary to explicitly construct the Json::Value object:
	root["encoding"] = getCurrentEncoding();
	root["indent"]["length"] = getCurrentIndentLength();
	root["indent"]["use_space"] = getCurrentIndentUseSpace();
	Json::StyledWriter writer;
	// Make a new JSON document for the configuration. Preserve original comments.
	std::string outputConfig = writer.write( root );
	// You can also use streams. This will put the contents of any JSON
	// stream at a particular sub-value, if you'd like.
	//std::cin >> root["subtree"];
	// And you can write to a stream, using the StyledWriter automatically.
	std::cout << root;

	return 0;
}
