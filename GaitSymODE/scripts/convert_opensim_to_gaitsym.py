#!/usr/bin/python -u

import sys
import string
import array
import os
import re
import time
import xml.etree.ElementTree
import scipy.interpolate

def convert_opensim_to_gaitsym():
    usageString = "Convert an OpenSim osim file to a GaitSym xml file\nUsage:%s in_config_osim in_vtp_folder reference_body out_config_xml out_obj_folder" % sys.argv[0]
    
    if len(sys.argv) != 6:
        print usageString
        exit(1)

    in_config_osim = sys.argv[1]
    in_vtp_folder = sys.argv[2]
    reference_body = sys.argv[3]
    out_config_xml = sys.argv[4]
    out_obj_folder = sys.argv[5]


    # some GaitSym defaults
    
    #IOCONTROL 
    
    OldStyleInputs = False
    
    # GLOBAL
    
    AllowInternalCollisions = False
    BMR = 0
    CFM = 1e-10
    ContactMaxCorrectingVel = 100
    ContactSurfaceLayer = 0.001
    DistanceTravelledBodyID = reference_body
    ERP = 0.2
    FitnessType = 'DistanceTravelled'
    GravityVector = '0.0 0.0 -9.81'
    IntegrationStepSize = 1e-4
    MechanicalEnergyLimit = 0
    MetabolicEnergyLimit = 0
    TimeLimit = 10

    # INTERFACE
    
    BodyAxisSize = '0.1 0.1 0.1'
    BodyColour = '.275 .725 .451 1.0'
    DrawingOrder = 'Environment Joint Muscle Geom Body'
    EnvironmentAxisSize = '1 1 1'
    EnvironmentColour = '0.5 0.5 1.0 1.0'
    GeomColour = '0 0 1 1'
    JointAxisSize = '0.05 0.05 0.05'
    JointColour = '0 1 0 1'
    StrapColour = '1 0 0 1'
    StrapCylinderColour = '0 1 1 1'
    StrapCylinderLength = 0.05
    StrapForceColour = '1 0 0 0.5'
    StrapForceRadius = 0.01
    StrapForceScale = 0.000001
    StrapRadius = 0.005
    TrackBodyID = reference_body
    
    # BODY
    AngularVelocity = "World 0 0 0"
    BadMesh = False
    Clockwise = False
    Density = -1
    GraphicFile = ''
    ID = 'UNDEFINED'
    LinearVelocity = 'World 0 0 0'
    LinearVelocityHighBound = '100 100 100'
    LinearVelocityLowBound = '-100 -100 -100'
    MOI = '1 1 1 0 0 0'
    Mass = 1
    Offset = '0 0 0'
    Position = 'World 0 0 0'
    PositionHighBound = '1000 1000 10'
    PositionLowBound = '-1000 -1000 0'
    Quaternion = 'World 0r 1 0 0'
    Scale = '1 1 1'

    # MUSCLE
    ActivationK = 0.17
    ActivationKinetics = False
    ActivationRate = 500
    FastTwitchProportion = 0.5
    FibreLength = 1
    ForcePerUnitArea = 300000
    ID = 'UNDEFINED'
    InitialFibreLength = 0
    Insertion = 'UNDEFINED'
    InsertionBodyID = 'UNDEFINED'
    Origin = 'UNDEFINED'
    OriginBodyID = 'UNDEFINED'
    PCA = 1
    ParallelStrainAtFmax = 0.6
    ParallelStrainModel = 'Square'
    ParallelStrainRateAtFmax = 0
    SerialStrainAtFmax = 0.06
    SerialStrainModel = 'Square'
    SerialStrainRateAtFmax = 0
    StartActivation = 0.001
    Strap = 'TwoPoint'
    TActivationA = 80e-3
    TActivationB = 0.47e-3
    TDeactivationA = 90e-3
    TDeactivationB = 0.56e-3
    TendonLength = -1
    Type_Muscle = 'MinettiAlexanderComplete'
    VMaxFactor = 8.4
    Width = 1

    # JOINT
    Body1ID = 'UNDEFINED'
    Body2ID = 'UNDEFINED'
    HingeAnchor = 'World 0 0 0'
    HingeAxis = 'World 0 0 1'
    ID = 'UNDEFINED'
    ParamHiStop = 1000
    ParamLoStop = -1000
    StartAngleReference = 0
    Type_Joint = 'Hinge'

    # start by parsin the osim XML file
    inputTree = xml.etree.ElementTree.parse(in_config_osim)
    inputRoot = inputTree.getroot()
    
    # and create an empty GAITSYM tree
    newTree = xml.etree.ElementTree.Element('GAITSYMODE')
    newTree.text = '\n'
    newTree.tail = '\n'
    
    newElement = xml.etree.ElementTree.Element('IOCONTROL')
    newElement.attrib['OldStyleInputs'] = str(OldStyleInputs)
    newElement.tail = '\n'
    newTree.append(newElement)

    # this loop for global settings
    for child in inputRoot:
        print child.tag
        if child.tag == 'Model':
            for model_data in child:
                print '    ' + model_data.tag
                if model_data.tag == 'DynamicsEngine':
                    for dynamics_data in model_data:
                        print '        ' + dynamics_data.tag
                        if dynamics_data.tag == 'SimbodyEngine':
                            for engine_data in dynamics_data:
                                if engine_data.tag == 'gravity':
                                    GravityVector = engine_data.text.strip()
    
    newElement = xml.etree.ElementTree.Element('GLOBAL')
    newElement.attrib['AllowInternalCollisions'] = str(AllowInternalCollisions)
    newElement.attrib['BMR'] = str(BMR)
    newElement.attrib['CFM'] = str(CFM)
    newElement.attrib['ContactMaxCorrectingVel'] = str(ContactMaxCorrectingVel)
    newElement.attrib['ContactSurfaceLayer'] = str(ContactSurfaceLayer)
    newElement.attrib['DistanceTravelledBodyID'] = str(DistanceTravelledBodyID)
    newElement.attrib['ERP'] = str(ERP)
    newElement.attrib['FitnessType'] = str(FitnessType)
    newElement.attrib['GravityVector'] = str(GravityVector)
    newElement.attrib['IntegrationStepSize'] = str(IntegrationStepSize)
    newElement.attrib['MechanicalEnergyLimit'] = str(MechanicalEnergyLimit)
    newElement.attrib['MetabolicEnergyLimit'] = str(MetabolicEnergyLimit)
    newElement.attrib['TimeLimit'] = str(TimeLimit)
    newElement.tail = '\n'
    newTree.append(newElement)

    newElement = xml.etree.ElementTree.Element('INTERFACE')
    newElement.attrib['BodyAxisSize'] = str(BodyAxisSize)
    newElement.attrib['BodyColour'] = str(BodyColour)
    newElement.attrib['DrawingOrder'] = str(DrawingOrder)
    newElement.attrib['EnvironmentAxisSize'] = str(EnvironmentAxisSize)
    newElement.attrib['EnvironmentColour'] = str(EnvironmentColour)
    newElement.attrib['GeomColour'] = str(GeomColour)
    newElement.attrib['JointAxisSize'] = str(JointAxisSize)
    newElement.attrib['JointColour'] = str(JointColour)
    newElement.attrib['StrapColour'] = str(StrapColour)
    newElement.attrib['StrapCylinderColour'] = str(StrapCylinderColour)
    newElement.attrib['StrapCylinderLength'] = str(StrapCylinderLength)
    newElement.attrib['StrapForceColour'] = str(StrapForceColour)
    newElement.attrib['StrapForceRadius'] = str(StrapForceRadius)
    newElement.attrib['StrapForceScale'] = str(StrapForceScale)
    newElement.attrib['StrapRadius'] = str(StrapRadius)
    newElement.attrib['TrackBodyID'] = str(TrackBodyID)
    newElement.tail = '\n'
    newTree.append(newElement)

    # this loop for bodies primarily but it collects joint data
    joint_data_list = {}
    for child in inputRoot:
        print child.tag
        if child.tag == 'Model':
            for model_data in child:
                print '    ' + model_data.tag
                if model_data.tag == 'DynamicsEngine':
                    for dynamics_data in model_data:
                        print '        ' + dynamics_data.tag
                        if dynamics_data.tag == 'SimbodyEngine':
                            for engine_data in dynamics_data:
                                if engine_data.tag == 'BodySet':
                                    for body_set_data in engine_data:
                                        if body_set_data.tag == 'objects':
                                            for object_data in body_set_data:
                                                if object_data.tag == 'Body':
                                                    ID = MakeLegalName(object_data.attrib['name'])
                                                    print '            ' + ID
                                                    inertia = [0,0,0,0,0,0]
                                                    location = [0,0,0]
                                                    parent_body = ''
                                                    coordinates = {}
                                                    geometry_files = []
                                                    GraphicFile = ''
                                                    for body_data in object_data:
                                                        if body_data.tag == 'mass_center': # this information not currently used
                                                            mass_center = body_data.text.strip()
                                                        if body_data.tag == 'mass':
                                                            Mass = float(body_data.text)
                                                        if body_data.tag == 'inertia_xx':
                                                            inertia[0] = float(body_data.text)
                                                        if body_data.tag == 'inertia_yy':
                                                            inertia[1] = float(body_data.text)
                                                        if body_data.tag == 'inertia_zz':
                                                            inertia[2] = float(body_data.text)
                                                        if body_data.tag == 'inertia_xy':
                                                            inertia[3] = float(body_data.text)
                                                        if body_data.tag == 'inertia_xz':
                                                            inertia[4] = float(body_data.text)
                                                        if body_data.tag == 'inertia_yz':
                                                            inertia[5] = float(body_data.text)
                                                        if body_data.tag == 'VisibleObject':
                                                            for visible_data in body_data:
                                                                if visible_data.tag == 'geometry_files':
                                                                    geometry_files = [s for s in visible_data.text.strip().split()]
                                                                    if len(geometry_files) <= 0:
                                                                        GraphicFile = ''
                                                                    else:
                                                                        GraphicFile = ID + '.obj'
                                                                        (vertex_list, normal_list, face_list) = ReadMultipleVTP(geometry_files, in_vtp_folder)
                                                                        if len(vertex_list) > 0:
                                                                            WriteOBJ(vertex_list, normal_list, face_list, ID, os.path.join(out_obj_folder, GraphicFile), False)
                                                                if visible_data.tag == 'scale_factors':
                                                                    Scale = visible_data.text.strip()
                                                        if body_data.tag == 'Joint':
                                                            for joint_data in body_data:
                                                                # print joint_data.tag
                                                                if joint_data.tag == 'CustomJoint' or joint_data.tag == 'WeldJoint':
                                                                    for custom_data in joint_data:
                                                                        # print custom_data.tag
                                                                        if custom_data.tag == 'parent_body':
                                                                            parent_body = MakeLegalName(custom_data.text)
                                                                        if custom_data.tag == 'location_in_parent':
                                                                            location_in_parent = [float(s) for s in custom_data.text.strip().split()]
                                                                        if custom_data.tag == 'location':
                                                                            original_location = [float(s) for s in custom_data.text.strip().split()]
                                                                            location = [float(s) for s in custom_data.text.strip().split()] # this may get transformed
                                                                        if custom_data.tag == 'CoordinateSet':
                                                                            for coordinate_set_data in custom_data:
                                                                                if coordinate_set_data.tag == 'objects':
                                                                                    for coordinate_set_object_data in coordinate_set_data:
                                                                                        if coordinate_set_object_data.tag == 'Coordinate':
                                                                                            coordinate_name = coordinate_set_object_data.attrib['name'].strip()
                                                                                            for coordinate_data in coordinate_set_object_data:
                                                                                                if coordinate_data.tag == 'default_value':
                                                                                                    coordinates[coordinate_name] = float(coordinate_data.text)
                                                                        if custom_data.tag == 'TransformAxisSet':
                                                                            for transform_set_data in custom_data:
                                                                                if transform_set_data.tag == 'objects':
                                                                                    for transform_set_object_data in transform_set_data:
                                                                                        #print transform_set_object_data.tag
                                                                                        if transform_set_object_data.tag == 'TransformAxis':
                                                                                            coordinate = 0
                                                                                            xx = []
                                                                                            yy = []
                                                                                            transform_name = transform_set_object_data.attrib['name']
                                                                                            for transform_data in transform_set_object_data:
                                                                                                #print transform_data
                                                                                                #print transform_data.text
                                                                                                if transform_data.tag == 'coordinate':
                                                                                                    coordinate = coordinates[transform_data.text.strip()]
                                                                                                if transform_data.tag == 'is_rotation':
                                                                                                    is_rotation = transform_data.text.strip().lower() in ['true', '1']
                                                                                                if transform_data.tag == 'axis': 
                                                                                                    axis = [float(s) for s in transform_data.text.strip().split()]
                                                                                                if transform_data.tag == 'function': 
                                                                                                    for function_data in transform_data:
                                                                                                        if function_data.tag == 'natCubicSpline':
                                                                                                            for spline_data in function_data:
                                                                                                                if spline_data.tag == 'x':
                                                                                                                    xx = [float(s) for s in spline_data.text.strip().split()]
                                                                                                                if spline_data.tag == 'y':
                                                                                                                    yy = [float(s) for s in spline_data.text.strip().split()]
                                                                                                
                                                                                            # apply the transformation
                                                                                            if len(xx) > 0:
                                                                                                coordinate = apply_function(xx, yy, coordinate)
                                                                                            if coordinate != 0:
                                                                                                if is_rotation:
                                                                                                    print 'Rotate %f along (%f,%f,%f) - WARNING NOT IMPLEMENTED' % (coordinate, axis[0],  axis[1],  axis[2])
                                                                                                else:
                                                                                                    print 'Translate %f along (%f,%f,%f)' % (coordinate, axis[0],  axis[1],  axis[2])
                                                                                                    location[0] -= coordinate * axis[0]
                                                                                                    location[1] -= coordinate * axis[1]
                                                                                                    location[2] -= coordinate * axis[2]
                                                                    if joint_data.tag == 'CustomJoint':
                                                                        joint_name = MakeLegalName(joint_data.attrib['name'])
                                                                        joint_data_list[joint_name] = (ID, parent_body, original_location) # joints seem to use the untransformed location
                                                                                      
                                                    
                                                    if ID == reference_body or parent_body == '':
                                                        Position = 'World %g %g %g' % (-location[0], -location[1], -location[2])
                                                    else:
                                                        Position = '%s %g %g %g %g %g %g' % (parent_body, location_in_parent[0], location_in_parent[1], location_in_parent[2], location[0], location[1], location[2])
                                                    
                                                    # some sanity checks
                                                    if Mass <= 0:
                                                        Mass = 1
                                                    if inertia[0] <= 0 and inertia[1] <= 0 and inertia[2] <= 0:
                                                        inertia[0] = 1
                                                        inertia[1] = 1
                                                        inertia[2] = 1

                                                    print 'BODY ID="%s" added' % ID
                                                    newElement = xml.etree.ElementTree.Element('BODY')

                                                    newElement.attrib['AngularVelocity'] = str(AngularVelocity)
                                                    newElement.attrib['Clockwise'] = str(Clockwise)
                                                    newElement.attrib['Density'] = str(Density)
                                                    newElement.attrib['GraphicFile'] = str(GraphicFile)
                                                    newElement.attrib['ID'] = str(ID)
                                                    newElement.attrib['LinearVelocity'] = str(LinearVelocity)
                                                    newElement.attrib['LinearVelocityHighBound'] = str(LinearVelocityHighBound)
                                                    newElement.attrib['LinearVelocityLowBound'] = str(LinearVelocityLowBound)
                                                    newElement.attrib['MOI'] = '%g %g %g %g %g %g' % (inertia[0], inertia[1], inertia[2], inertia[3], inertia[4], inertia[5])
                                                    newElement.attrib['Mass'] = str(Mass)
                                                    newElement.attrib['Offset'] = str(Offset)
                                                    newElement.attrib['Position'] = str(Position)
                                                    newElement.attrib['PositionHighBound'] = str(PositionHighBound)
                                                    newElement.attrib['PositionLowBound'] = str(PositionLowBound)
                                                    newElement.attrib['Quaternion'] = str(Quaternion)
                                                    newElement.attrib['Scale'] = str(Scale)

                                                    newElement.tail = '\n'
                                                    newTree.append(newElement)

    
    # this loop for joints
    joint_names = joint_data_list.keys()
    for joint_name in joint_names:
        (Body1ID, Body2ID, location) = joint_data_list[joint_name]
        #HingeAnchor = '%s %g %g %g' % (Body1ID, -location[0], -location[1], -location[2])
        HingeAnchor = '%s %g %g %g' % (Body1ID, location[0], location[1], location[2])
        ID = joint_name;
        print 'Creating JOINT ID="%s"' % ID
        newElement = xml.etree.ElementTree.Element('JOINT')
        newElement.attrib['Body1ID'] = str(Body1ID)
        newElement.attrib['Body2ID'] = str(Body2ID)
        newElement.attrib['HingeAnchor'] = str(HingeAnchor)
        newElement.attrib['HingeAxis'] = str(HingeAxis)
        newElement.attrib['ID'] = str(ID)
        newElement.attrib['ParamHiStop'] = str(ParamHiStop)
        newElement.attrib['ParamLoStop'] = str(ParamLoStop)
        newElement.attrib['StartAngleReference'] = str(StartAngleReference)
        newElement.attrib['Type'] = str(Type_Joint)
        newElement.tail = '\n'
        newTree.append(newElement)
    
    # this loop for muscles
    for child in inputRoot:
        print child.tag
        if child.tag == 'Model':
            for model_data in child:
                print '    ' + model_data.tag
                if model_data.tag == 'ActuatorSet':
                    for actuator_data in model_data:
                        print '        ' + actuator_data.tag
                        if actuator_data.tag == 'objects':
                            for object_data in actuator_data:
                                print '            ' + object_data.tag
                                ID = MakeLegalName(object_data.attrib['name'])
                                print '            ' + ID
                                for muscle_data in object_data:
                                    # print muscle_data.tag
                                    if muscle_data.tag == 'max_isometric_force':
                                        PCA = float(muscle_data.text) / ForcePerUnitArea
                                    if muscle_data.tag == 'optimal_fiber_length':
                                        FibreLength = float(muscle_data.text)
                                    if muscle_data.tag == 'tendon_slack_length':
                                        TendonLength = float(muscle_data.text)
                                    if muscle_data.tag == 'pennation_angle': # this information not currently used
                                        pennation_angle = float(muscle_data.text)
                                    if muscle_data.tag == 'MusclePointSet':
                                        point_locations = []
                                        point_bodies = []
                                        for point_set_data in muscle_data:
                                            # print point_set_data.tag
                                            if point_set_data.tag == 'objects':
                                                for point_data in point_set_data:
                                                    if point_data.tag == 'MusclePoint' or point_data.tag == 'MuscleViaPoint' or point_data.tag == 'MovingMusclePoint':
                                                        location = point_data.findtext('location', '')
                                                        if len(location):
                                                            body = point_data.findtext('body', '')
                                                            if len(body):
                                                                point_locations.append(location.strip())
                                                                point_bodies.append(MakeLegalName(body))
                                                        XAttachment = point_data.find('XAttachment')
                                                        if XAttachment is not None:
                                                            natCubicSpline = XAttachment.find('natCubicSpline')
                                                            x_text = natCubicSpline.findtext('x', '')
                                                            y_text = natCubicSpline.findtext('y', '')
                                                            xx = [float(s) for s in x_text.strip().split()]
                                                            yy = [float(s) for s in y_text.strip().split()]
                                                            x_pos = apply_function(xx, yy, 0)
                                                            YAttachment = point_data.find('YAttachment')
                                                            natCubicSpline = YAttachment.find('natCubicSpline')
                                                            x_text = natCubicSpline.findtext('x', '')
                                                            y_text = natCubicSpline.findtext('y', '')
                                                            xx = [float(s) for s in x_text.strip().split()]
                                                            yy = [float(s) for s in y_text.strip().split()]
                                                            y_pos = apply_function(xx, yy, 0)
                                                            ZAttachment = point_data.find('ZAttachment')
                                                            natCubicSpline = ZAttachment.find('natCubicSpline')
                                                            x_text = natCubicSpline.findtext('x', '')
                                                            y_text = natCubicSpline.findtext('y', '')
                                                            xx = [float(s) for s in x_text.strip().split()]
                                                            yy = [float(s) for s in y_text.strip().split()]
                                                            z_pos = apply_function(xx, yy, 0)
                                                            point_locations.append('%f %f %f' % (x_pos, y_pos, z_pos))
                                                            body = point_data.findtext('body', '')
                                                            point_bodies.append(MakeLegalName(body))
                                                    else:
                                                        print 'MUSCLE ID="%s" %s unprocessed' % (ID, point_data.tag)
                                        Origin = '%s %s' % (point_bodies[0], point_locations[0])
                                        OriginBodyID = '%s' % (point_bodies[0])
                                        Insertion = '%s %s' % (point_bodies[-1], point_locations[-1])
                                        InsertionBodyID = '%s' % (point_bodies[-1])
                                        ViaPoints = []
                                        ViaPointBodyIDs = []
                                        if len(point_bodies) == 2:
                                            Strap = 'TwoPoint'
                                        else:
                                            Strap = 'NPoint'
                                            for i in range(1, len(point_bodies) - 1):
                                                ViaPoints.append('%s %s' % (point_bodies[i], point_locations[i]))
                                                ViaPointBodyIDs.append('%s' % (point_bodies[i]))
                                    

                                newElement = xml.etree.ElementTree.Element('MUSCLE')
                                
                                newElement.attrib['ActivationK'] = str(ActivationK)
                                newElement.attrib['ActivationKinetics'] = str(ActivationKinetics)
                                newElement.attrib['ActivationRate'] = str(ActivationRate)
                                newElement.attrib['FastTwitchProportion'] = str(FastTwitchProportion)
                                newElement.attrib['FibreLength'] = str(FibreLength)
                                newElement.attrib['ForcePerUnitArea'] = str(ForcePerUnitArea)
                                newElement.attrib['ID'] = str(ID)
                                newElement.attrib['InitialFibreLength'] = str(InitialFibreLength)
                                newElement.attrib['Insertion'] = str(Insertion)
                                newElement.attrib['InsertionBodyID'] = str(InsertionBodyID)
                                newElement.attrib['Origin'] = str(Origin)
                                newElement.attrib['OriginBodyID'] = str(OriginBodyID)
                                newElement.attrib['PCA'] = str(PCA)
                                newElement.attrib['ParallelStrainAtFmax'] = str(ParallelStrainAtFmax)
                                newElement.attrib['ParallelStrainModel'] = str(ParallelStrainModel)
                                newElement.attrib['ParallelStrainRateAtFmax'] = str(ParallelStrainRateAtFmax)
                                newElement.attrib['SerialStrainAtFmax'] = str(SerialStrainAtFmax)
                                newElement.attrib['SerialStrainModel'] = str(SerialStrainModel)
                                newElement.attrib['SerialStrainRateAtFmax'] = str(SerialStrainRateAtFmax)
                                newElement.attrib['StartActivation'] = str(StartActivation)
                                newElement.attrib['Strap'] = str(Strap)
                                newElement.attrib['TActivationA'] = str(TActivationA)
                                newElement.attrib['TActivationB'] = str(TActivationB)
                                newElement.attrib['TDeactivationA'] = str(TDeactivationA)
                                newElement.attrib['TDeactivationB'] = str(TDeactivationB)
                                newElement.attrib['TendonLength'] = str(TendonLength)
                                newElement.attrib['Type'] = str(Type_Muscle)
                                newElement.attrib['VMaxFactor'] = str(VMaxFactor)
                                newElement.attrib['Width'] = str(Width)
                            
                                for i in range(0, len(ViaPoints)):
                                    newElement.attrib['ViaPoint%d' % i] = str(ViaPoints[i])
                                    newElement.attrib['ViaPoint%dBodyID' % i] = str(ViaPointBodyIDs[i])
                            
                                newElement.tail = '\n'
                                newTree.append(newElement)

    out = open(out_config_xml, 'w')
    out.write(xml.etree.ElementTree.tostring(newTree))
    out.close();

def WriteOBJ(vertex_list, normal_list, face_list, group_name, filename, relative_flag):
    fout = open(filename, 'w')
    fout.write('# File generated by %sn\n' % sys.argv[0])

    fout.write('g %s\n' % group_name);
    fout.write('o %s\n' % group_name);

    # vertices first
    for vertex in vertex_list:
        fout.write('v %f %f %f\n' % (vertex[0], vertex[1], vertex[2]))

    # normals next
    for normal in normal_list:
        fout.write('vn %f %f %f\n' % (normal[0], normal[1], normal[2]))

    # finally faces
    if relative_flag == False:
        offset = 1
    else:
        offset = - len(vertex_list)
    for face in face_list:
        line = 'f '
        for index in face:
            line = '%s %d//%d' % (line, index + offset, index + offset)
        fout.write(line + '\n')

    fout.close()


# GaitSym IDs cannot start with a number but OpenSim one can (and do)
def MakeLegalName(name):
    name = name.strip();
    if '0123456789'.find(name[0]) != -1:
        return 'NNN_' + name
    return name;

def ReadMultipleVTP(filename_list, folder):
    
    full_vertex_list = []
    full_normal_list = []
    full_face_list = []
    for filename in filename_list:
        (vertex_list, normal_list, face_list) = ReadVTP(os.path.join(folder, filename))
        offset = len(full_vertex_list)
        for vertex in vertex_list:
            full_vertex_list.append(vertex)
        for normal in normal_list:
            full_normal_list.append(normal)
        for face in face_list:
            new_face = []
            for index in face:
                new_face.append(index + offset)
            full_face_list.append(new_face)
    return (full_vertex_list, full_normal_list, full_face_list)


def ReadVTP(filename):

    vertex_list = []
    normal_list = []
    face_list = []
    
    if os.path.isfile(filename):

        # start by parsin the vtp XML file
        print "Reading %s" % filename
        inputTree = xml.etree.ElementTree.parse(filename)
        inputRoot = inputTree.getroot()
    
        # this loop for global settings
        for file_data in inputRoot:
            #print file_data.tag
            if file_data.tag == 'PolyData':
                for poly_data in file_data:
                    #print poly_data.tag
                    if poly_data.tag == 'Piece':
                        for piece_data in poly_data:
                            #print piece_data.tag
                            if piece_data.tag == 'PointData':
                                for point_data in piece_data:
                                    #print point_data.tag
                                    if point_data.tag == 'DataArray':
                                        Name = point_data.attrib['Name']
                                        if Name == 'Normals':
                                            tokens = ConvertToListOfTokens(point_data.text)
                                            for i in range(0, len(tokens), 3):
                                                normal_list.append([float(tokens[i]), float(tokens[i + 1]), float(tokens[i + 2])])
                            if piece_data.tag == 'Points':
                                for point_data in piece_data:
                                    if point_data.tag == 'DataArray':
                                        tokens = ConvertToListOfTokens(point_data.text)
                                        for i in range(0, len(tokens), 3):
                                            vertex_list.append([float(tokens[i]), float(tokens[i + 1]), float(tokens[i + 2])])
                            if piece_data.tag == 'Polys':
                                connectivity = []
                                offsets = [0]
                                for polys_data in piece_data:
                                    if polys_data.tag == 'DataArray':
                                        Name = polys_data.attrib['Name']
                                        if Name == 'connectivity':
                                            tokens = ConvertToListOfTokens(polys_data.text)
                                            for i in range(0, len(tokens)):
                                                connectivity.append(int(tokens[i]))
                                        if Name == 'offsets':
                                            tokens = ConvertToListOfTokens(polys_data.text)
                                            for i in range(0, len(tokens)):
                                                offsets.append(int(tokens[i]))
                                for i in range(1, len(offsets)):
                                    face = []
                                    for j in range(offsets[i - 1], offsets[i]):
                                        face.append(connectivity[j])
                                    face_list.append(face)
    
    else:
        print "Warning %s not found" % filename
                        
    return (vertex_list, normal_list, face_list)

def apply_function(xx, yy, x):

    print xx, yy
    w = None
    bbox = [None, None]
    k=3
    if (len(xx) == 3):
        k = 2
    if (len(xx) == 2):
        k = 1
    s = scipy.interpolate.InterpolatedUnivariateSpline(xx, yy, w, bbox, k)
    y = s(x)

    return y

def ConvertFromListOfTokens(tokens):
    s = ''
    for i in range(0, len(tokens) - 1):
        s = s + tokens[i] + ' '
    s = s + tokens[-1]
    return s

def NegateString(s):
    s = s.strip()
    if s.startswith('-'):
        s = s[1:]
    else:
        if s.startswith('+'):
            s = s[1:]
        s = '-' + s
    return s

    
def ConvertToListOfTokens(charList):
    """converts a string into a list of tokens delimited by whitespace"""
    i = 0
    tokenList = []
    while i < len(charList):
        byte = charList[i]
        if byte in string.whitespace: 
            i = i + 1
            continue
        word = ""
        if byte == '"':
            i = i + 1
            byte = charList[i]
            while byte != '"':
                word = word + byte
                i = i + 1
                if i >= len(charList): PrintExit("Unmatched \" error")
                byte = charList[i]
        else:
            while (byte in string.whitespace) == 0:
                word = word + byte
                i = i + 1
                if i >= len(charList): break
                byte = charList[i]
        
        if len(word) > 0: tokenList.append(word)
        i = i + 1
    return tokenList        

def GetIndexFromTokenList(tokenList, match):
    """returns the index pointing to the value in the tokenList 
       return -1 if not found"""
    for i in range(0, len(tokenList)):
        if tokenList[i] == match: return i
    return -1

def ConvertFileToTokens(filename):
    """reads in a file and converts to a list of whitespace delimited tokens"""
    input = open(filename, 'r')
    theBuffer = input.read()
    input.close()
    return ConvertToListOfTokens(theBuffer)

# program starts here

if __name__ == '__main__':
    convert_opensim_to_gaitsym()
    
