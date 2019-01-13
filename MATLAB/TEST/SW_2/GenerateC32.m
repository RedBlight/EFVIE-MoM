function [ C32 ] = GenerateC32( faceCount, kontrastDiff )
	
	% C32( n )
	
	C32 = zeros( faceCount, 1 );
	
	K = 1 / ( 4 * pi * M.eps0 );
	
	for idFace = 1 : faceCount
		
		C32( idFace ) = - K * kontrastDiff( idFace );
		
	end
	
end

