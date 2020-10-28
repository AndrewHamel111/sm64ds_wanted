// no raylib allowed here! ya understand?1?!?!?

#include "network.hpp"
#include "HTTPRequest.hpp"

std::string getBodyFromURL(std::string url)
{
	std::string str;
	
	http::Request request(url);
	const http::Response response = request.send("GET");
	str = std::string(response.body.begin(), response.body.end());
	
	return str;
}

bool sendPostRequest(std::string url, std::string post)
{	
	try
	{
		http::Request request(url);
		const http::Response response = request.send("POST", post, {"Content-Type: application/x-www-form-urlencoded"});
		
		return true;
	}
	catch (const std::exception& e)
	{
		// do nothin
		return false;
	}
	
	return false;
}