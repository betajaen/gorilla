require 'YAML'

sources = []
headers = []
target_source = 'main.cpp'
target_header = 'main.h'
wrapper_source = 'wrapper.cpp'
wrapper_header = 'wrapper.h'
namespace = 'Application'

config = YAML::load(DATA)
config["sources"].each do |fn|
	
	if fn.match('.cpp\/h$')
		fn = fn[0..fn.rindex('.')]
		sources.push fn + 'cpp'
		headers.push fn + 'h'
	elsif fn.match('.cpp$')
		sources.push fn
	elsif fn.match('.h$')
		headers.push fn
	end
end

target_source = config["as"] + '.cpp'
target_header = config["as"] + '.h'
namespace = config["namespace"]

class String
	def strip_to_first_of(c)
		v = index(c)
		return "" if v == nil || v == length
		return slice(0,v)
	end

	def strip_after_the_first_of(c)
		v = index(c)
		return "" if v == nil || v == length
		return slice(v+1,length)
	end

end

def parse_file(fn, namespace)
	
	f = File.read(fn)
	in_block = false
	level = 0
	parse_this = false
	src = ""
	f.each_line do |ln|

		last_level = level
		level = level + ln.count('{')
		level = level - ln.count('}')
		
		if in_block == false
			if ln =~ /^\s*namespace #{namespace}/
				in_block = true
			else
				next	
			end
		end

		if (last_level == 0 && level == 1)
			ln = ln.strip_after_the_first_of('{')
			parse_this = true
		end

		if (last_level == 1 && level <= 0)
			ln = ln.strip_to_first_of('}')
			in_block == false
			parse_this = true
		end

		if (level > 0 || parse_this)

			if (parse_this && ln.strip.length == 0)
				parse_this = false
				next
			end

			parse_this = false
			src << ln

		end

	end

	return src
end

##########################################################


src = ""
sources.each do |fn|
	next if File.new(fn).size == 0
	src << parse_file(fn, namespace)
end

src_final = File.read(wrapper_source)
src_final.gsub!(/^\s*\/\/SOURCE-CODE\s*/, src)

File.open(target_source, 'w' ) {|f| f.write(src_final) }
puts "Wrote source code to #{target_source}"

hdr = ""
headers.each do |fn|
	next if File.new(fn).size == 0
	hdr << parse_file(fn, namespace)
end

headers_final = File.read(wrapper_header)
headers_final.gsub!(/^\s*\/\/SOURCE-CODE\s*/, hdr)

File.open(target_header, 'w' ) {|f| f.write(headers_final) }

puts "Wrote source code to #{target_header}"

__END__
sources:
  - Shared.cpp/h
  - ook.cpp
  - Gorilla.cpp/h
  - Resources.cpp/h
  - Canvas.cpp/h
  - OverlayCanvas.cpp/h
  - MovableCanvas.cpp/h
wrapper: Wrapper
as: ../Gorilla
namespace: Cake