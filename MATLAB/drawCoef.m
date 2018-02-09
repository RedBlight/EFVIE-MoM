clc;
clear all;
close all;

% A = [ 2 .* pi .* (0 : 360) ./ 360; repmat( pi, 1, 361 ) ];
% 
% A = double( reshape( A, 1, 722 ) );
% 
% fileId = fopen( 'Data/xycircular361.obsangle', 'w' );
% 
% fwrite( fileId, 361, 'int64' );
% fwrite( fileId, A, 'double' );



fileID = fopen( 'Data/sphere52r1.rhs' );
coefCount = fread( fileID, 1, 'int64' );
% coef = fread( fileID, coefCount*2, 'double' );
% coef = reshape( coef, [2,coefCount] );
% 
fclose( fileID );
% 
% 
% figure();
% plot( 1:coefCount, coef(1,:) );
% 
% figure();
% plot( 1:coefCount, coef(2,:) );

coef = ones( 2, coefCount );

coef(1,:) = ones( 1, coefCount );
coef(2,:) = zeros( 1, coefCount );
coef = reshape( coef, [1,2*coefCount] );


fileID = fopen( 'Data/sphere52r1.coef', 'w' );

fwrite( fileID, coefCount, 'int64' );
fwrite( fileID, coef, 'double' );

fclose( fileID );

%coef = coef(1,:) + 1i .* coef(2,:);





%fclose( fileID );
