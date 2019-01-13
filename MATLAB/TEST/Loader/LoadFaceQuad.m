function [ faceCount, quadCount, quadData ] = LoadFaceQuad( fileName )
    
    fileID = fopen( "Data/" + fileName + ".triquad" );
    
    faceCount = fread( fileID, 1, 'uint64' );
    
    quadCount = fread( fileID, 1, 'uint64' );
    
    quadData = fread( fileID, faceCount * quadCount * 4, 'double' );
    quadData = transpose( reshape( quadData, [ 4, faceCount * quadCount ] ) );
    
    fclose( fileID );
end

