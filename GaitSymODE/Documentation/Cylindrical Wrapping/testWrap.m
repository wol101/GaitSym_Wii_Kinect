function testWrap()

originDistance = 60
insertionDistance = 40
frameCount = 0;
radius = 25
tension = 10
nExtraPathCoordinates = 10
% maxAngle = 1.25 * pi % suggest values from pi to 2 * pi
maxAngle = 1 * pi
height = 120

for insertionAngle = 0: 10: 360
	for originAngle = 0: 10: 360
      originAngle2 = originAngle * pi / 180
      insertionAngle2 = insertionAngle * pi / 180
      origin = [originDistance * cos(originAngle2), originDistance * sin(originAngle2), 0]
      insertion= [insertionDistance * cos(insertionAngle2), insertionDistance * sin(insertionAngle2), height]

		[wrapOK, originForce, insertionForce, cylinderForce, cylinderForcePosition, pathLength, pathCoordinates] = ...
   		CylinderWrap(origin, insertion, radius, tension, nExtraPathCoordinates, maxAngle);
      
		figure(1);
		clf reset;
		hold on;
      
      % Environment
		[x, y, z] = cylinder(radius, 20);
		surf(x, y, height * z,'FaceColor','None');
		[x, y, z] = sphere(10);
		surf(origin(1)+x, origin(2)+y, origin(3)+z,'FaceColor','None');
      surf(insertion(1)+x, insertion(2)+y, insertion(3)+z,'FaceColor','None');
      
      % Path
		line(pathCoordinates(:,1),pathCoordinates(:,2),pathCoordinates(:,3),'Color','Red');
      
      % Forces
      line([origin(1), origin(1) + originForce(1)],...
         [origin(2), origin(2) + originForce(2)],...
         [origin(3), origin(3) + originForce(3)], 'Color','Blue');
      line([insertion(1), insertion(1) + insertionForce(1)],...
         [insertion(2), insertion(2) + insertionForce(2)],...
         [insertion(3), insertion(3) + insertionForce(3)], 'Color','Blue');
      line([cylinderForcePosition(1), cylinderForcePosition(1) + cylinderForce(1)],...
         [cylinderForcePosition(2), cylinderForcePosition(2) + cylinderForce(2)],...
         [cylinderForcePosition(3), cylinderForcePosition(3) + cylinderForce(3)],...
         'Color','Blue');
      
		axis equal;
      axis([-60,60,-60,60,0,height]);
		xlabel('X');
		ylabel('Y');
      zlabel('Z');
      view(0, 90); % default 2D
      % view(-37.5, 30); % default 3D
      
      frameCount = frameCount + 1
      frames(frameCount) = getframe;
   end
end

movie(frames, 15)
