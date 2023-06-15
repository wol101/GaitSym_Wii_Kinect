function TestTwoCylinderWrap()

figure(1)
clf('reset');
set(gcf, 'Color', 'w');
set(gcf,'Units','pixels');
set(gcf, 'Position', [100, 100, 800, 800], 'Toolbar', 'none');


originDistance = 110
insertionDistance = 80
frameCount = 0;

cylinderPosition1.x = 20
cylinderPosition1.y = -10
cylinderPosition1.z = 0
radius1 = 30

cylinderPosition2.x = -35
cylinderPosition2.y = 15
cylinderPosition2.z = 0
radius2 = 25

tension = 10
nPointsPerCylinderArc = 16
% maxAngle = 1.25 * pi % suggest values from pi to 2 * pi
maxAngle = 1 * pi

% aviobj = avifile('two_cylinder_movie.avi','compression','None', 'fps', 25);
fig = figure(1);

for originAngle = 0: 10: 360
    for insertionAngle = 0: 10: 360
        originAngle2 = originAngle * pi / 180
        insertionAngle2 = insertionAngle * pi / 180
        
        origin.x = originDistance * cos(originAngle2)
        origin.y = originDistance * sin(originAngle2)
        origin.z = 0;
        insertion.x = insertionDistance * cos(insertionAngle2)
        insertion.y = insertionDistance * sin(insertionAngle2)
        insertion.z = 0;
        
        [wrapOK, originForce, insertionForce, cylinderForce1, cylinderForcePosition1, cylinderForce2, cylinderForcePosition2, pathLength, pathCoordinates] = ...
            TwoCylinderWrap(origin, insertion, cylinderPosition1, radius1, cylinderPosition2, radius2, tension, nPointsPerCylinderArc, pi());

        
        figure(1);
        clf;
        
        draw_circle(cylinderPosition1, radius1, 100, 'blue')
        draw_circle(cylinderPosition2, radius2, 100, 'red')
        
        line(pathCoordinates(:, 1), pathCoordinates(:, 2), 'Color', 'black','LineWidth', 2)
        
        
        axis equal;
        axis([-120,120,-120,120]);
        xlabel('X');
        ylabel('Y');
        
        % F = getframe(fig);
        % aviobj = addframe(aviobj, F);
    end
end

% aviobj = close(aviobj);

return

function draw_circle(centre, radius, segments, colour)

x = zeros(segments + 1, 1);
y = zeros(segments + 1, 1);
for i = 1: segments    
    angle = (i / segments) * 2 * pi();
    x(i) = radius * cos(angle) + centre.x;
    y(i) = radius * sin(angle) + centre.y;
end
x(segments + 1) = x(1);
y(segments + 1) = y(1);
line(x, y, 'Color', colour);

return


    
    
