function [ relTT, relTS, relSS ] = GenerateElemRel( faceCount, tetraCount, tetraVertexIndex, faceVertexIndex )
	
	relTT = false( tetraCount, tetraCount );
	relTS = false( tetraCount, faceCount );
	relSS = false( faceCount, faceCount );
	
	for idTetraA = 1 : tetraCount
		vertA = tetraVertexIndex( :, idTetraA );
		for idTetraB = 1 : tetraCount
			vertB = tetraVertexIndex( :, idTetraB );
			isSingTT = false;
			for iva = 1 : 4
				for ivb = 1 : 4
					isSingTT = isSingTT || vertA( iva ) == vertB( ivb );
				end
			end
			relTT( idTetraA, idTetraB ) = isSingTT;
		end
		for idFaceN = 1 : faceCount
			vertN = faceVertexIndex( :, idFaceN );
			isSingTS = false;
			for iva = 1 : 4
				for ivn = 1 : 3
					isSingTS = isSingTS || vertA( iva ) == vertN( ivn );
				end
			end
			relTS( idTetraA, idFaceN ) = isSingTS;
		end
	end
	
	for idFaceM = 1 : faceCount
		vertM = faceVertexIndex( :, idFaceM );
		for idFaceN = 1 : faceCount
			vertN = faceVertexIndex( :, idFaceN );
			isSingSS = false;
			for ivm = 1 : 3
				for ivn = 1 : 3
					isSingSS = isSingSS || vertM( ivm ) == vertN( ivn );
				end
			end
			relSS( idFaceM, idFaceN ) = isSingSS;
		end
	end
	
end