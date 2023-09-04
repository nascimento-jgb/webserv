#!/usr/bin/ruby

puts "Content-Type: text/html; charset=UTF-8"
puts
puts "<!DOCTYPE html>"
puts "<html>"
puts "<head>"
puts "    <title>CGI Script in Ruby</title>"
puts "</head>"
puts "<body>"
puts "    <h1>Hello from a CGI Script in Ruby!</h1>"
puts "    <p>This is an example of a CGI script written in Ruby.</p>"
puts "    <p>The current time is: #{Time.now.strftime('%Y-%m-%d %H:%M:%S')}</p>"
puts "</body>"
puts "</html>"