function [ C31 ] = GenerateC31( faceCount, faceTetraIndex, kontrast, faceArea, tetraVol )
	
	% C31( n, b )
	% b := { 1, 2 }
	
	C31 = zeros( faceCount, 2 );
	
	K = 1 / ( 4 * pi * M.eps0 );
	
	for idFace = 1 : faceCount
		
		idTet1 = faceTetraIndex( 1, idFace );
		idTet2 = faceTetraIndex( 2, idFace );
		
		faceAr = faceArea( idFace );
		
		tetVol1 = tetraVol( idTet1 );
		kon1 = kontrast( idTet1 );
		
		C31( idFace, 1 ) = K * kon1 * faceAr / tetVol1;
		
		if idTet2 ~= -1
			
			tetVol2 = tetraVol( idTet2 );
			kon2 = kontrast( idTet2 );

			C31( idFace, 2 ) = K * kon2 * faceAr / tetVol2;
			
		end
		
	end
	
end

