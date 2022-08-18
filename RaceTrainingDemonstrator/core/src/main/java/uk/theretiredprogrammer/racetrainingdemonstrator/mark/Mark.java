/*
 * Copyright 2014-2017 Richard Linsdale.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
package uk.theretiredprogrammer.racetrainingdemonstrator.mark;

import java.awt.BasicStroke;
import java.awt.Color;
import java.awt.Graphics2D;
import java.awt.geom.AffineTransform;
import java.awt.geom.Ellipse2D;
import java.awt.geom.Line2D;
import uk.theretiredprogrammer.racetrainingdemonstrator.core.api.Location;
import uk.theretiredprogrammer.racetrainingdemonstrator.core.api.DisplayableElement;
import uk.theretiredprogrammer.racetrainingdemonstrator.core.api.Direction;
import uk.theretiredprogrammer.racetrainingdemonstrator.core.api.FlowElement;
import uk.theretiredprogrammer.racetrainingdemonstrator.core.api.KeyPair;
import uk.theretiredprogrammer.racetrainingdemonstrator.core.api.KeyPair.Status;
import static uk.theretiredprogrammer.racetrainingdemonstrator.core.api.KeyPair.Status.BADKEY;
import static uk.theretiredprogrammer.racetrainingdemonstrator.core.api.KeyPair.Status.BADVALUE;
import static uk.theretiredprogrammer.racetrainingdemonstrator.core.api.KeyPair.Status.OK;
import uk.theretiredprogrammer.racetrainingdemonstrator.core.api.Polar;

/**
 * The Mark Class - represent course marks.
 *
 * @author Richard Linsdale (richard at theretiredprogrammer.uk)
 */
public class Mark extends DisplayableElement {

    final private FlowElement wind;

    // parameters which can be set via the definition file
    private Location pos = new Location();
    private boolean downwindLaylines = false;
    private boolean windwardLaylines = false;
    private double laylineLength = 0;
    private Color colour = Color.RED;
    private Color laylineColour = Color.black;
    private boolean leavetoport = true;
    private String nextMark = "";
    //
    private Location laylineBase;
    //
    private final double size = 1; // set up as 1 metre diameter object
    //

    /**
     * Constructor
     *
     * @param name the name
     * @param wind the wind flow to be applied
     */
    public Mark(String name, FlowElement wind) {
        super(name);
        this.wind = wind;
        setLaylineBase();
    }

    /**
     * Test if the mark as left to Port.
     *
     * @return true if leave to port
     */
    public boolean toPort() {
        return leavetoport;
    }

    /**
     * Get the mark location.
     *
     * @return the location
     */
    public Location getLocation() {
        return pos;
    }

    /**
     * Get polar from given location to this mark.
     *
     * @param boatpos the given location
     * @return the polar
     */
    public Polar getTruePolar(Location boatpos) {
        return new Polar(pos, boatpos);
    }

    /**
     * Get a Polar course to the next mark. Include room to pass mark and
     * understanding tacking up and down wind.
     *
     * @param boatpos the current boat position
     * @param boatDirection the current boat direction
     * @param reachdownwind true if boat tacks downwind
     * @param boatoffset the boat offset when rounding mark
     * @param distance allowance for rounding
     * @return the polar
     */
    public Polar getCoursePolar(Location boatpos, Direction boatDirection, boolean reachdownwind, double boatoffset, double distance) {   // actually is the location to pass the mark
        Polar boatmove = new Polar(distance, boatDirection); // this the potential boat movement during direction change
        Polar tomark = new Polar(pos, boatpos);
        tomark.subtract(boatmove);
        Direction w = wind.getFlow(pos).getDirection();
        int relative = tomark.absAngleDiff(w);
        if (relative <= 45) {
            return tomark.add(new Polar((size + boatoffset) * 2, w.getDegrees() + (leavetoport ? 90 : -90)));
        } else if (relative >= 135 && reachdownwind) {
            return tomark.add(new Polar((size + boatoffset) * 2, w.getDegrees() + 180 + (leavetoport ? 90 : -90)));
        } else {
            return tomark.add(new Polar((size + boatoffset) * Math.sqrt(2), boatDirection.getDegrees() + (leavetoport ? 90 : -90)));
        }
    }

    /**
     * Get the next mark name
     *
     * @return the next mark name
     */
    public String nextMark() {
        return nextMark;
    }

    @Override
    public void timerAdvance(int time) {
        setLaylineBase();
    }

    private void setLaylineBase() {
        // calculate the target point as the present time for current wind direction
        Direction windDirection = wind.getFlow(pos).getDirection();
        Polar toPt;
        if (downwindLaylines) {
            toPt = new Polar(2 * size, windDirection.getDegrees()
                    + (leavetoport ? -90 : +90));
        } else {
            toPt = new Polar(2 * size, windDirection.getDegrees()
                    + (leavetoport ? 90 : -90));
        }
        laylineBase = toPt.polar2Location(pos);
    }

    @Override
    public void draw(Graphics2D g2D, double pixelsPerMetre) {
        double radius;
        if (pixelsPerMetre > 6) {
            // if it will be visible then draw circle to scale
            radius = size / 2 * pixelsPerMetre;
        } else {
            radius = 3; // create visible object
        }
        Ellipse2D s = new Ellipse2D.Double(-radius, -radius, radius * 2, radius * 2);
        AffineTransform xform = g2D.getTransform();
        g2D.translate(pos.x, pos.y);
        g2D.scale(1 / pixelsPerMetre, 1 / pixelsPerMetre);
        g2D.setColor(colour);
        g2D.draw(s);
        g2D.fill(s);
        g2D.setTransform(xform);
        // now draw the laylines - this are scale independent and set to 1 pixel line
        Direction windDirection = wind.getFlow(pos).getDirection();
        if (windwardLaylines) {
            pixelLine(g2D, laylineBase, new Polar(laylineLength,
                    windDirection.getDegrees() + 135), laylineColour, pixelsPerMetre);
            pixelLine(g2D, laylineBase, new Polar(laylineLength,
                    windDirection.getDegrees() - 135), laylineColour, pixelsPerMetre);
        }
        if (downwindLaylines) {
            pixelLine(g2D, laylineBase, new Polar(laylineLength,
                    windDirection.getDegrees() + 45), laylineColour, pixelsPerMetre);
            pixelLine(g2D, laylineBase, new Polar(laylineLength,
                    windDirection.getDegrees() - 45), laylineColour, pixelsPerMetre);
        }
    }

    private void pixelLine(Graphics2D g2D, Location laylineBase, Polar line,
            Color laylineColour, double pixelsPerMetre) {
        BasicStroke stroke = new BasicStroke((float) (1f / pixelsPerMetre));
        g2D.setStroke(stroke);
        g2D.setColor(laylineColour);
        Location end = line.polar2Location(laylineBase);
        g2D.draw(new Line2D.Double(laylineBase.x, laylineBase.y, end.x, end.y));
    }

    @Override
    protected Status setParameter(KeyPair kp) {
        try {
            switch (kp.key) {
                case "location":
                    pos = parseLocation(kp.value);
                    setLaylineBase();
                    return OK;
                case "windwardlaylines":
                    windwardLaylines = parseYesNo(kp.value);
                    setLaylineBase();
                    return OK;
                case "downwindlaylines":
                    downwindLaylines = parseYesNo(kp.value);
                    setLaylineBase();
                    return OK;
                case "laylinelength":
                    laylineLength = Double.parseDouble(kp.value);
                    return OK;
                case "laylinecolour":
                    laylineColour = parseColour(kp.value);
                    return OK;
                case "colour":
                    colour = parseColour(kp.value);
                    return OK;
                case "leavetoport":
                    leavetoport = parseYesNo(kp.value);
                    setLaylineBase();
                    return OK;
                case "nextmark":
                    nextMark = kp.value;
                    return OK;
                default:
                    return BADKEY;
            }
        } catch (NumberFormatException numberFormatException) {
            return BADVALUE;
        }
    }

    @Override
    protected Status checkParameter(KeyPair kp) {
        try {
            switch (kp.key) {
                case "location":
                    parseLocation(kp.value);
                    return OK;
                case "windwardlaylines":
                    parseYesNo(kp.value);
                    return OK;
                case "downwindlaylines":
                    parseYesNo(kp.value);
                    return OK;
                case "laylinelength":
                    Double.parseDouble(kp.value);
                    return OK;
                case "laylinecolour":
                    parseColour(kp.value);
                    return OK;
                case "colour":
                    Color chk = parseColour(kp.value);
                    return OK;
                case "leavetoport":
                    parseYesNo(kp.value);
                    return OK;
                case "nextmark":
                    return OK;
                default:
                    return BADKEY;
            }
        } catch (NumberFormatException numberFormatException) {
            return BADVALUE;
        }
    }
}
