function [ vertexData, tetraVertexIndex ] = ReadUnv( unvFileName )

	vertexStartStr = '  2411';
	vertexEndStr = '    -1';
	indexStartStr = '  2412';
	indexEndStr = '    -1';
	
	vertexCount = 0;
	tetraCount = 0;
	
	unvFile = fopen( unvFileName );
	lineStr = fgetl( unvFile );
	
	while ~strcmp( lineStr, vertexStartStr )
		
	end
	
	
end

