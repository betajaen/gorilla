#
# BMFontGen to Gorilla file generator
#
# Description
#      Converts BMFontGen files (xml) into Gorilla files.
#
# Usage
#	   > bmfontgen_to_gorilla font.xml


def make_gorilla(xm, name = nil)

	glyphs = []
	kernings = []
	baseline = 0
	spacelength = 0
	lineheight = 0
	kerning = 0
	monowidth = 0
	alt_name = ""
	texture_name = "texture.png"
	range_begin = 65535
	range_end = 0

	xm.each_line do |l|
		
		if l.include? "<glyph "
			
			# <glyph ch="!" code="0021" bm="0" origin="1,0" size="8x23" aw="7" lsb="0" />

			glyph = {}

			c = l.match /code="(\w+)"/
			if (c != nil)
				glyph[:id] = c[1].to_i(16)
			else
				next
			end

			origin = l.match /origin="(\d+,\d+)"/
			if (origin != nil)
				x, y = origin[1].split(',')
				glyph[:x] = x.to_i
				glyph[:y] = y.to_i
			end

			size = l.match /size="(\d+x\d+)"/
			if (size != nil)
				w, h = size[1].split('x')
				glyph[:w] = w.to_i
				glyph[:h] = h.to_i
				monowidth = glyph[:w] if (monowidth < glyph[:w])
			end

			advance = l.match /aw="(\d+)"/
			if (size != nil)
				glyph[:advance] = advance[1].to_i

				if (glyph[:id] == 32)
					spacelength = advance[1].to_i
					next
				end

			end

			if (glyph[:id] < range_begin)
				range_begin = glyph[:id]
			end

			if (glyph[:id] > range_end)
				range_end = glyph[:id]
			end
			
			glyphs.push glyph

			next
		elsif l.include? "<kernpair "
			
			k = {}

			# <kernpair left="-" right="J" adjust="1" />

			lft = l.match /left="(.)"/
			if (lft != nil)
				k[:left] = lft[1].to_s.ord
			end

			rgt = l.match /right="(.)"/
			if (rgt != nil)
				k[:right] = rgt[1].to_s.ord
			end

			kn = l.match /adjust="(-?\d+)"/
			if (kn != nil)
				k[:adjust] = kn[1].to_i
			end

			kernings.push k

			next
		end

		# base="18"
		bl = l.match /base="(\d+)"/
		if (bl != nil)
			baseline = bl[1].to_i
		end

		# height="18"
		lh = l.match /height="(\d+)"/
		if (lh != nil)
			lineheight = lh[1].to_i
		end

		# size="14"
		sz = l.match /size="(\d+)"/
		if (sz != nil)
			alt_name = sz[1]
		end


		# name="d-0.png"
		nm = l.match /<bitmap.+name="([\w\.\-]+)"/
		if (nm != nil)
			texture_name = nm[1]
		end
	end

	gorilla = String.new
	gorilla << "[Texture]\nfile #{texture_name}\nwhitepixel 0 0\n"

	if name != nil
		gorilla << "\n[Font." << name << "]\n"
	else
		gorilla << "\n[Font." << alt_name << "]\n"
	end

	gorilla << "lineheight " << lineheight.to_s << "\n"
	gorilla << "spacelength " << spacelength.to_s << "\n"
	gorilla << "baseline " << baseline.to_s << "\n"
	gorilla << "kerning " << kerning.to_s << "\n"
	gorilla << "monowidth " << monowidth.to_s << "\n"
	gorilla << "range " << range_begin.to_s << " " << range_end.to_s << "\n"

	glyphs.each do |glyph|
		gorilla << "glyph_" << glyph[:id].to_s << " " << glyph[:x].to_s  << " " << glyph[:y].to_s  << " " << glyph[:w].to_s << " " << glyph[:h].to_s << " " << glyph[:advance].to_s  << "\n" 
	end

	kernings.each do |k|
		gorilla << "kerning_" << k[:right].to_s << " " << k[:left].to_s << " " << k[:adjust].to_s << "\n" 
	end

	gorilla
end


File.open(ARGV[0].to_s.gsub('.xml', '.gorilla'), 'w' ) {|f| f.write(make_gorilla(File.read ARGV[0].to_s) ) }
puts "Wrote to #{ARGV[0].to_s.gsub('.xml', '.gorilla')}"