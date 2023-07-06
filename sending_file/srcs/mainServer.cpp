/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mainServer.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: joaonascimento <joaonascimento@student.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/06 07:11:59 by joaonascime       #+#    #+#             */
/*   Updated: 2023/07/06 07:12:22 by joaonascime      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/socketServer.hpp"

int main()
{
	SocketServer SocketServer;
	std::string requestedFile = "/info.html";  // The requested file path.
	std::string rootDirectory = "/Users/joaonascimento/Desktop/webserv-local/server";

	// Check if the requested file exists in the server's root directory.
	std::string filePath = rootDirectory + requestedFile;  // Construct the absolute file path.
	std::string fileContent = readFile(filePath);

	if (!fileContent.empty()) {
		// File found - Send the file to the client.
		std::string responseHeader = "HTTP/1.1 200 OK\r\n"
									 "Content-Type: text/html\r\n"
									 "Content-Length: " + intToString(fileContent.size()) + "\r\n"
									 "\r\n";

		std::cout << responseHeader << fileContent << std::endl;  // Send the response to the client.
	} else {
		// File not found - Return a 404 response.
		std::string notFoundResponse = "HTTP/1.1 404 Not Found\r\n"
									   "Content-Type: text/plain\r\n"
									   "Content-Length: 13\r\n"
									   "\r\n"
									   "File not found";
		std::cout << notFoundResponse << std::endl;  // Send the 404 response to the client.
	}
	SocketServer.startServer();
	return (0);
}
