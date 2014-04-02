/*
 * Copyright (C) 2014 Richard Linsdale (richard.linsdale at blueyonder.co.uk)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
package linsdale.rtd.boat;

import java.awt.BasicStroke;
import java.awt.Color;
import java.awt.Graphics2D;
import java.awt.Shape;
import java.awt.geom.AffineTransform;
import java.awt.geom.GeneralPath;
import java.awt.geom.Line2D;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import linsdale.nbpcg.annotations.RegisterLog;
import linsdale.nbpcg.supportlib.LogicException;
import linsdale.rtd.core.api.RTAObject;
import linsdale.rtd.core.api.Direction;
import linsdale.rtd.core.api.Flow;
import linsdale.rtd.core.api.Location;
import linsdale.rtd.core.api.Polar;
import linsdale.rtd.code.DefFileModel;
import linsdale.nbpcg.supportlib.OutputReporter;
import linsdale.rtd.code.Scenario;
import linsdale.rtd.core.api.FlowModel;
import linsdale.rtd.mark.Mark;

/**
 *
 * @author Richard Linsdale (richard.linsdale at blueyonder.co.uk)
 */
@RegisterLog("linsdale.rtd.boat")
public abstract class Boat extends RTAObject {
    // parameters - can be set via the definition file

    private Color colour = Color.black;
    private final Color sailColour = Color.white;
    private Color trackColour = Color.black;
    //
    private boolean tackonbesttack = false;
    private boolean tackifheaded = false;
    private boolean bearawayupwindifheaded = false;
    private boolean luffupupwindiflifted = false;
    //
    private boolean reachdownwind = false;
    private boolean gybeonbesttack = false;
    private boolean gybeiflifted = false;
    private boolean bearawaydownwindifheaded = false;
    private boolean luffupdownwindiflifted = false;
    //
    private boolean upwindchannel = false;
    private double upwindchannelleft = 0;
    private double upwindchannelright = 0;
    private boolean downwindchannel = false;
    private double downwindchannelleft = 0;
    private double downwindchannelright = 0;
    // actual boat performance metrics - passed to module durng initialisation - metrics(...) and performance(...)
    private double length;// metres
    private double width;// metres
    private double inertia;// fraction of speed lost or gained in changing winds
    private int maxTurningAnglePerSecond;
    private int upwindrelative;
    private int downwindrelative;
    private int windAngleData[];
    private double windSpeedData[];
    private double boatSpeedData[][];
    // THESE ARE THE SET OF VARIABLES WHICH ARE RECALCULATED FOR EVERY MOVE
    // initial state - at beginning of the time step (and updated after move)
    private Location pos = new Location(); // can also be defined by definition file
    private Direction direction = new Direction();// can also be defined by definition file
    private double boatspeed = 0;
    private int rotationAnglePerSecond;
    private int closeangle;
    private int currentMode = SAILON;
    // calculated during timestep to define the required direction of the move
    private Direction nextDirection = new Direction();
    private int turnDirection = 0;
    // set of variables calculated at the start of every timestep - for calculation - based of environment at the time
    private Direction winddirection;
    private Polar sailToNextMark;
    private int markAngleToWind;
    private int boatAngleToWind;
    private int windAngleToMeanWind;
    private double distanceToMark;
    private int angleToNextMark;
    private Polar fromStartOfLeg;
    private int currentTack;
    private double downwindchannelvalue;
    private double upwindchannelvalue;
    // THESE VARIABLES ARE CALCULATED FOR EVERY LEG 
    private Mark nextMark;
    private Polar nextMarkPolar;
    private Location startOfLeg;
    private Mark followingMark;
    private Polar followingMarkPolar;
    private boolean allowcorrection = false;
    //
    private double clearance; // clearance when sailing to mark
    private double close; // distance from mark at which mark rounding  routing rules are applied
    //
    private static final int SAILON = 0;
    private static final int STOPPED = 1;
    private static final int MARKROUNDING = 2;
    private static final int TURN = 3;
    //
    private static final int PORT = -1;
    private static final int STARBOARD = 1;
    //
    private final List<Location> track = Collections.synchronizedList(new ArrayList<Location>());

    public Boat(String name, OutputReporter reporter, DefFileModel dfm) {
        super(name, reporter, dfm);
        initNextMarkVariables();
    }

    public void metrics(double length, double width, double inertia, int maxTurningAnglePerSecond,
            int upwindrelative, int downwindrelative) {
        this.length = length;
        close = length * 3;
        this.width = width;
        clearance = width * 2;
        this.inertia = inertia;
        this.maxTurningAnglePerSecond = maxTurningAnglePerSecond;
        rotationAnglePerSecond = maxTurningAnglePerSecond / 2;
        closeangle = 90 - rotationAnglePerSecond / 2;
        this.upwindrelative = upwindrelative;
        this.downwindrelative = downwindrelative;
    }

    public void performance(double boatSpeedData[][], int windAngleData[], double windSpeedData[]) {
        this.boatSpeedData = boatSpeedData;
        this.windAngleData = windAngleData;
        this.windSpeedData = windSpeedData;
    }

    @Override
    public void finish() {
        nextMark = null;
        followingMark = null;
        super.finish();
    }

    @Override
    public void timerAdvance(int time) {
        calculateAllEnvironmentVariables();
        if (currentMode == SAILON) {
            currentMode = limitsHandler();
            if (currentMode == SAILON) {
                currentMode = nextMark == null ? simpleSailDecisionMaker() : sailDecisionMaker();
            }
        }
        switch (currentMode) {
            case SAILON:
                moveBoat(direction);
                break;
            case STOPPED:
                break; // obviously don't move boat
            case MARKROUNDING:
                turn();
                if (currentMode == SAILON) {
                    changeNextMarkVariables();
                }
                break;
            case TURN:
                turn();
                break;
            default:
                throw new LogicException("Illegal sailing Mode when moving boat");
        }
    }

    private void calculateAllEnvironmentVariables() {
        FlowModel windmodel = dfm.getWind();
        winddirection = windmodel.getFlow(pos).getDirection();
        currentTack = direction.angleDiff(winddirection) > 0 ? STARBOARD : PORT;
        if (nextMark != null) {
            Polar toNextMark = nextMark.getTruePolar(pos);
            sailToNextMark = new Polar(toNextMark);
            sailToNextMark.add(new Polar(clearance, toNextMark.getAngle() + (nextMark.toPort() ? +90 : -90)));
            markAngleToWind = winddirection.absAngleDiff(sailToNextMark.getDirection());
            distanceToMark = toNextMark.getDistance();
            angleToNextMark = direction.angleDiff(toNextMark.getDirection()) * (nextMark.toPort() ? -1 : 1);
            fromStartOfLeg = new Polar(pos, startOfLeg);
            downwindchannelvalue = currentTack == STARBOARD ? downwindchannelleft : downwindchannelright;
            upwindchannelvalue = currentTack == STARBOARD ? upwindchannelleft : upwindchannelright;
        }
        windAngleToMeanWind = winddirection.angleDiff(windmodel.getMeanFlow(pos).getDirection());
        boatAngleToWind = direction.absAngleDiff(winddirection);
    }

    private void turn() {
        if (direction.absAngleDiff(nextDirection) <= rotationAnglePerSecond) {
            moveBoat(nextDirection);
            currentMode = SAILON;
        } else {
            moveBoat(new Direction(direction, turnDirection * rotationAnglePerSecond));
        }
    }

    private int limitsHandler() {
        // test if at fixed limits and 'relect' direction to keep within limits
        // test for a distance inside limits so that turn is complete within limits.
        Scenario scenario = dfm.getScenario();
        double distance = boatspeed * 4;
        int angle = direction.getDegrees();
        if (pos.x <= (scenario.getWestLimit() + distance) && angle < 0) {
            nextDirection = new Direction(-angle);
            turnDirection = angle > -90 ? STARBOARD : PORT;
            return TURN;
        }
        if (pos.x >= (scenario.getEastLimit() - distance) && angle > 0) {
            nextDirection = new Direction(-angle);
            turnDirection = angle > 90 ? STARBOARD : PORT;
            return TURN;
        }
        if (pos.y >= (scenario.getNorthLimit() - distance) && Math.abs(angle) < 90) {
            nextDirection = new Direction((angle > 0 ? 180 : -180) - angle);
            turnDirection = angle > 0 ? STARBOARD : PORT;
            return TURN;
        }
        if (pos.y <= (scenario.getSouthLimit() + distance) && Math.abs(angle) > 90) {
            nextDirection = new Direction((angle > 0 ? 180 : -180) - angle);
            turnDirection = angle < 0 ? STARBOARD : PORT;
            return TURN;
        }
        return SAILON;
    }

    private int simpleSailDecisionMaker() {
        if (boatAngleToWind < upwindrelative) {
            nextDirection = new Direction(winddirection, -currentTack * upwindrelative);
            turnDirection = currentTack;
            return TURN;
        }
        return SAILON;
    }

    private int sailDecisionMaker() {
        if (distanceToMark <= close) {
            if (angleToNextMark < 0) { // need to tack here
                nextDirection = new Direction(winddirection, currentTack * upwindrelative);
                turnDirection = currentTack;
                return TURN;
            }
            if (followingMark == null) {
                return angleToNextMark > 90 ? STOPPED : SAILON;
            }
            turnDirection = nextMark.toPort() ? PORT : STARBOARD;
            if (boatAngleToWind < 90 && turnDirection == currentTack) {
                if (angleToNextMark >= closeangle + 30) {// allow more rounding space if tacking
                    nextDirection = new Direction(winddirection, nextMark.toPort() ? -upwindrelative : upwindrelative);
                    return TURN;
                } else {
                    if (boatAngleToWind < upwindrelative) {
                        nextDirection = new Direction(winddirection, -currentTack * upwindrelative);
                        turnDirection = -currentTack;
                        return TURN;
                    } else {
                        return SAILON;
                    }
                }
            }
            if (angleToNextMark >= closeangle) {
                nextDirection = followingMarkPolar.getDirection();
                int nextAngle = winddirection.absAngleDiff(nextDirection);
                if (nextAngle < upwindrelative) {
                    nextDirection = new Direction(winddirection, nextMark.toPort() ? upwindrelative : -upwindrelative);
                }
                return MARKROUNDING;
            } else {
                if (boatAngleToWind < upwindrelative) {
                    nextDirection = new Direction(winddirection, -currentTack * upwindrelative);
                    turnDirection = -currentTack;
                    return TURN;
                } else {
                    return SAILON;
                }
            }
        }

        if (markAngleToWind < upwindrelative) {
            // test if we are to stay within upwind channel
            if (upwindchannel && distanceToMark > -upwindchannelvalue * 1.5) {
                int angleMarkToNext = fromStartOfLeg.angleDiff(nextMarkPolar);
                double offset = fromStartOfLeg.getDistance() * Math.sin(Math.toRadians(currentTack * angleMarkToNext));
                if (offset > upwindchannelvalue) {
                    nextDirection = new Direction(winddirection, currentTack * upwindrelative);
                    turnDirection = currentTack;
                    return TURN;
                }
            }
            // check if need to tack onto best tack
            if (tackonbesttack && currentTack * windAngleToMeanWind > 0) {
                nextDirection = new Direction(winddirection, currentTack * upwindrelative);
                turnDirection = currentTack;
                return TURN;
            }
            // check if need to tack
            if (tackifheaded && boatAngleToWind < upwindrelative) {
                nextDirection = new Direction(winddirection, currentTack * upwindrelative);
                turnDirection = currentTack;
                return TURN;
            }
            // check if need to bear away 
            if (bearawayupwindifheaded && boatAngleToWind < upwindrelative) {
                nextDirection = new Direction(winddirection, -currentTack * upwindrelative);
                turnDirection = -currentTack;
                return TURN;
            }
            // check if need to luff
            if (luffupupwindiflifted && boatAngleToWind > upwindrelative) {
                nextDirection = new Direction(winddirection, -currentTack * upwindrelative);
                turnDirection = currentTack;
                return TURN;
            }
            return SAILON;
        }
        if (markAngleToWind > downwindrelative && reachdownwind) {
            // test if we are to stay within downwindwind channel
            if (downwindchannel && distanceToMark > -downwindchannelvalue * 1.5) {
                int angleMarkToNext = fromStartOfLeg.angleDiff(nextMarkPolar);
                double offset = fromStartOfLeg.getDistance() * Math.sin(Math.toRadians(-currentTack * angleMarkToNext));
                if (offset > downwindchannelvalue) {
                    nextDirection = new Direction(winddirection, currentTack * downwindrelative);
                    turnDirection = -currentTack;
                    return TURN;
                }
            }
            // check if need to gybe onto best tack
            if (gybeonbesttack && currentTack * windAngleToMeanWind < 0) {
                nextDirection = new Direction(winddirection, currentTack * downwindrelative);
                turnDirection = -currentTack;
                return TURN;
            }
            // check if need to gybe
            if (gybeiflifted && boatAngleToWind > downwindrelative) {
                nextDirection = new Direction(winddirection, currentTack * downwindrelative);
                turnDirection = -currentTack;
                return TURN;
            }
            // check if need to luff
            if (luffupdownwindiflifted && boatAngleToWind > downwindrelative) {
                nextDirection = new Direction(winddirection, -currentTack * downwindrelative);
                turnDirection = currentTack;
                return TURN;
            }
            // check if need to bear away 
            if (bearawaydownwindifheaded && boatAngleToWind < downwindrelative) {
                nextDirection = new Direction(winddirection, -currentTack * downwindrelative);
                turnDirection = -currentTack;
                return TURN;
            }
            return SAILON;
        }
        // offwind - reaching/running
        if (allowcorrection || distanceToMark > close) {
            nextDirection = sailToNextMark.getDirection();
            int nextAngle = winddirection.angleDiff(nextDirection);
            if (currentTack * nextAngle >= 0) {
                allowcorrection = true;
                if (boatAngleToWind < 90) {
                    nextDirection = new Direction(winddirection, currentTack * upwindrelative);
                    turnDirection = currentTack;
                } else {
                    turnDirection = -currentTack;
                }
                return TURN;
            }
            allowcorrection = false;
            if (currentTack * direction.angleDiff(nextDirection) < 0) {
                turnDirection = -currentTack;
                return TURN;
            }
            if (currentTack * direction.angleDiff(nextDirection) > 0) {
                turnDirection = currentTack;
                return TURN;
            }
        }
        return SAILON;
    }

    private void initNextMarkVariables() {
        if (dfm != null) {
            Scenario scenario = dfm.getScenario();
            nextMark = (Mark) dfm.getNamedInstance(scenario.getFirstMark());
            updateNextMarkVariables();
        }
    }

    private void changeNextMarkVariables() {
        nextMark = followingMark;
        updateNextMarkVariables();
    }

    private void updateNextMarkVariables() {
        if (nextMark != null) {
            startOfLeg = new Location(pos);
            nextMarkPolar = new Polar(nextMark.getLocation(), startOfLeg);
            followingMark = (Mark) dfm.getNamedInstance(nextMark.nextMark());
            if (followingMark != null) {
                followingMarkPolar = new Polar(followingMark.getLocation(), nextMark.getLocation());
            }
        }
    }

    public void moveBoat(Direction requiredDirection) {
        Flow wind = dfm.getWind().getFlow(pos);
        Flow water = dfm.getWater().getFlow(pos);
        // calculate the potential boat speed - based on wind speed and relative angle 
        double potentialBoatspeed = Flow.convertKnots2MetresPerSecond(
                getPotentialBoatSpeed(requiredDirection.absAngleDiff(wind.getDirection()),
                        wind.getSpeedKnots()));
        boatspeed += inertia * (potentialBoatspeed - boatspeed);
        // and now move the boat
        // start by calculating the vector components of the boats movement
        Polar move = new Polar(boatspeed, requiredDirection);
        // next calculate and add the components of water movement
        move.subtract(
                new Polar(water.getSpeedMetresPerSecond(), water.getDirection()));
        Location updated = move.polar2Location(pos); // updated position calculated
        track.add(updated); // record it in track
        pos.set(updated); // and set the new position
        direction = requiredDirection; // and update the direction
        rotationAnglePerSecond = boatspeed < 1 ? maxTurningAnglePerSecond / 2 : maxTurningAnglePerSecond;
        closeangle = 90 - rotationAnglePerSecond / 2;
    }

    private double getPotentialBoatSpeed(int relative, double windSpeed) {
        // find the lookup point for wind direction
        int angleindex = -1;
        for (int i = 0; i < windAngleData.length; i++) {
            if (windAngleData[i] > relative) { // found higher
                break;
            }
            angleindex++;
        }
        // find the lookup point for wind speed
        int speedindex = -1;
        for (int i = 0; i < windSpeedData.length; i++) {
            if (windSpeedData[i] > windSpeed) { // found higher
                break;
            }
            speedindex++;
        }
        // now extrapolate the boat speed, firstly on two windspeed curves and then between them.
        double boatspeedlower = extrapolateFromTable(boatSpeedData, windAngleData, relative, speedindex, angleindex);
        double boatspeedupper = extrapolateFromTable(boatSpeedData, windAngleData, relative, speedindex + 1, angleindex);
        return extrapolateSpeed(boatspeedlower, boatspeedupper, windSpeed, windSpeedData, speedindex);
    }

    private double extrapolateFromTable(double[][] boatSpeedData, int[] windAngleData, int relative,
            int speedindex, int angleindex) {
        double boatspeedlower = boatSpeedData[speedindex][angleindex];
        double boatspeedupper = boatSpeedData[speedindex][angleindex + 1];
        double ratio = ((double) (relative - windAngleData[angleindex])) / (windAngleData[angleindex + 1] - windAngleData[angleindex]);
        return boatspeedlower + ratio * (boatspeedupper - boatspeedlower);
    }

    private double extrapolateSpeed(double boatspeedlower, double boatspeedupper, double windSpeed,
            double[] windSpeedData, int speedindex) {
        double ratio = (windSpeed - windSpeedData[speedindex]) / (windSpeedData[speedindex + 1] - windSpeedData[speedindex]);
        return boatspeedlower + ratio * (boatspeedupper - boatspeedlower);
    }

    @Override
    public int setParameter(String key, String value) {
        try {
            switch (key) {
                case "heading":
                    int heading = Integer.parseInt(value);
                    direction.set(heading);
                    nextDirection.set(heading);
                    break;
                case "location":
                    pos = parseLocation(value);
                    updateNextMarkVariables();
                    break;
                case "colour":
                    colour = parseColour(value);
                    trackColour = colour;
                    break;
                case "trackcolour":
                    trackColour = parseColour(value);
                    break;
                case "upwindsailonbesttack":
                    tackonbesttack = parseYesNo(value);
                    break;
                case "upwindtackifheaded":
                    tackifheaded = parseYesNo(value);
                    break;
                case "upwindbearawayifheaded":
                    bearawayupwindifheaded = parseYesNo(value);
                    break;
                case "upwindluffupiflifted":
                    luffupupwindiflifted = parseYesNo(value);
                    break;
                case "reachdownwind":
                    reachdownwind = parseYesNo(value);
                    break;
                case "downwindsailonbestgybe":
                    gybeonbesttack = parseYesNo(value);
                    break;
                case "downwindbearawayifheaded":
                    bearawaydownwindifheaded = parseYesNo(value);
                    break;
                case "downwindgybeiflifted":
                    gybeiflifted = parseYesNo(value);
                    break;
                case "downwindluffupiflifted":
                    luffupdownwindiflifted = parseYesNo(value);
                    break;
                case "upwindchannel":
                    double vu = Double.parseDouble(value) / 2;
                    upwindchannelleft = vu;
                    upwindchannelright = vu;
                    upwindchannel = true;
                    break;
                case "upwindchannelleft":
                    upwindchannelleft = Double.parseDouble(value);
                    upwindchannel = true;
                    break;
                case "upwindchannelright":
                    upwindchannelright = Double.parseDouble(value);
                    upwindchannel = true;
                    break;
                case "downwindchannel":
                    double vd = Double.parseDouble(value) / 2;
                    downwindchannelleft = vd;
                    downwindchannelright = vd;
                    downwindchannel = true;
                    break;
                case "downwindchannelleft":
                    downwindchannelleft = Double.parseDouble(value);
                    downwindchannel = true;
                    break;
                case "downwindchannelright":
                    downwindchannelright = Double.parseDouble(value);
                    downwindchannel = true;
                    break;
                default:
                    return PARAM_BADKEY;
            }
            return PARAM_OK;
        } catch (NumberFormatException numberFormatException) {
            return PARAM_BADVALUE;
        }
    }

    @Override
    public int checkParameter(String key, String value) {
        try {
            switch (key) {
                case "heading":
                    Integer.parseInt(value);
                    break;
                case "location":
                    parseLocation(value);
                    break;
                case "colour":
                    parseColour(value);
                    break;
                case "trackcolour":
                    parseColour(value);
                    break;
                case "upwindsailonbesttack":
                    parseYesNo(value);
                    break;
                case "upwindtackifheaded":
                    parseYesNo(value);
                    break;
                case "upwindbearawayifheaded":
                    parseYesNo(value);
                    break;
                case "upwindluffupiflifted":
                    parseYesNo(value);
                    break;
                case "reachdownwind":
                    parseYesNo(value);
                    break;
                case "downwindsailonbestgybe":
                    parseYesNo(value);
                    break;
                case "downwindbearawayifheaded":
                    parseYesNo(value);
                    break;
                case "downwindgybeiflifted":
                    parseYesNo(value);
                    break;
                case "downwindluffupiflifted":
                    parseYesNo(value);
                    break;
                case "upwindchannel":
                    Double.parseDouble(value);
                    break;
                case "upwindchannelleft":
                    Double.parseDouble(value);
                    break;
                case "upwindchannelright":
                    Double.parseDouble(value);
                    break;
                case "downwindchannel":
                    Double.parseDouble(value);
                    break;
                case "downwindchannelleft":
                    Double.parseDouble(value);
                    break;
                case "downwindchannelright":
                    Double.parseDouble(value);
                    break;
                default:
                    return PARAM_BADKEY;
            }
            return PARAM_OK;
        } catch (NumberFormatException numberFormatException) {
            return PARAM_BADVALUE;
        }
    }

    @Override
    public void draw(Graphics2D g2D, double pixelsPerMetre) {
        int relative = direction.angleDiff(dfm.getWind().getFlow(pos).getDirection());
        boolean onStarboard = relative > 0;
        int absrelative = Math.abs(relative);
        int sailRotation = absrelative <= 45 ? 0 : (absrelative - 45) * 2 / 3;
        double l;
        double w;
        if (pixelsPerMetre > 4) {
            l = length * pixelsPerMetre;
            w = width * pixelsPerMetre;
        } else {
            l = 13; // create a visible object
            w = 5; // create a visible object
        }
        GeneralPath p = new GeneralPath();
        p.moveTo(-l * 0.5, w * 0.45); //go to transom quarter
        p.curveTo(-l * 0.2, w * 0.55, l * 0.2, l * 0.1, l * 0.5, 0);
        p.curveTo(l * 0.2, -l * 0.1, -l * 0.2, -w * 0.55, -l * 0.5, -w * 0.45);
        p.closePath(); // and the port side
        GeneralPath sail = new GeneralPath();
        if (onStarboard) {
            sail.moveTo(0.0, 0);
            sail.curveTo(-l * 0.2, l * 0.1, -l * 0.4, w * 0.4, -l * 0.7, w * 0.4);
            sailRotation = -sailRotation;
        } else {
            sail.moveTo(0, 0);
            sail.curveTo(-l * 0.2, -l * 0.1, -l * 0.4, -w * 0.4, -l * 0.7, -w * 0.4);
        }
        //
        AffineTransform xform = g2D.getTransform();
        g2D.translate(pos.x, pos.y);
        g2D.scale(1 / pixelsPerMetre, 1 / pixelsPerMetre);
        g2D.rotate(Math.toRadians(90 - direction.getDegrees()));
        g2D.setColor(colour);
        g2D.draw(p);
        g2D.fill(p);
        g2D.translate(l * 0.2, 0);
        g2D.rotate(Math.toRadians(sailRotation));
        g2D.setColor(sailColour);
        g2D.setStroke(new BasicStroke(2));
        g2D.draw(sail);
        g2D.setTransform(xform);

        double MetresPerPixel = 1 / pixelsPerMetre;
        BasicStroke stroke = new BasicStroke((float) (MetresPerPixel));
        BasicStroke heavystroke = new BasicStroke((float) (MetresPerPixel * 3));
        g2D.setColor(trackColour);
        int count = 0;
        synchronized (track) {
            for (Location tp : track) {
                g2D.setStroke(count % 60 == 0 ? heavystroke : stroke);
                Shape s = new Line2D.Double(tp.x, tp.y, tp.x, tp.y);
                g2D.draw(s);
                count++;
            }
        }
    }
}
