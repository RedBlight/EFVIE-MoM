function [ kontrastDiff ] = GenerateKontrastDiff( kontrast, faceCount, faceTetraIndex )
	
	kontrastDiff = zeros( faceCount, 1 );
	
	for idFace = 1 : faceCount
		
		idTetP = faceTetraIndex( 1, idFace );
		idTetM = faceTetraIndex( 2, idFace );
		
		kontrastDiff( idFace ) = kontrast( idTetP );
		if idTetM ~= -1
			kontrastDiff( idFace ) = kontrastDiff( idFace ) - kontrast( idTetM );
		end
		
	end
	
end