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
package uk.org.rlinsdale.racetrainingdemonstrator.complexflow;

import java.awt.BasicStroke;
import java.awt.Color;
import java.awt.Font;
import java.awt.Graphics2D;
import java.awt.geom.AffineTransform;
import java.awt.geom.GeneralPath;
import java.text.NumberFormat;
import uk.org.rlinsdale.racetrainingdemonstrator.core.api.Flow;
import uk.org.rlinsdale.racetrainingdemonstrator.core.api.FlowElement;
import uk.org.rlinsdale.racetrainingdemonstrator.core.api.Location;
import uk.org.rlinsdale.racetrainingdemonstrator.core.ScenarioElement;
import uk.org.rlinsdale.racetrainingdemonstrator.core.api.KeyPair;
import uk.org.rlinsdale.racetrainingdemonstrator.core.api.KeyPair.Status;
import static uk.org.rlinsdale.racetrainingdemonstrator.core.api.KeyPair.Status.BADKEY;
import static uk.org.rlinsdale.racetrainingdemonstrator.core.api.KeyPair.Status.BADVALUE;
import static uk.org.rlinsdale.racetrainingdemonstrator.core.api.KeyPair.Status.OK;

/**
 * The ComplexFlow Class - represents a flow which is described by flows (speed
 * and direction) at the four corners points. Flows within the described area
 * are interpolated.
 *
 * @author Richard Linsdale (richard.linsdale at blueyonder.co.uk)
 */
public class ComplexFlow extends FlowElement {

    /**
     * the North East location (top right)
     */
    protected Location northeast = new Location();

    /**
     * the flow at the North East location
     */
    protected Flow northeastFlow = new Flow();

    /**
     * the North West location (top left)
     */
    protected Location northwest = new Location();

    /**
     * the flow at the North West location
     */
    protected Flow northwestFlow = new Flow();

    /**
     * the South East location (bottom right)
     */
    protected Location southeast = new Location();

    /**
     * the flow at the South East location 
     */
    protected Flow southeastFlow = new Flow();

    /**
     * the South West location (bottom left)
     */
    protected Location southwest = new Location();

    /**
     * the flow at the South West location
     */
    protected Flow southwestFlow = new Flow();
    private boolean showFlow = false;
    private double showFlowInterval = 100;
    private Color showFlowColour = Color.BLACK;
    private int swing = 10;
    private int period = 120;
    private int shift = 10;
    private int shiftperiod = 120;
    private boolean randomshifts = false;
    //
    private boolean swinging = false;
    private boolean shifting = false;
    private int swingNow = 0;
    private int shiftNow = 0;
    //
    private final double eastedge;
    private final double westedge;
    private final double northedge;
    private final double southedge;

    /**
     * Constructor
     * 
     * @param name the instance name to be associated with the flow
     * @param scenario the field of play
     */
    public ComplexFlow(String name, ScenarioElement scenario) {
        super(name, scenario);
        eastedge = scenario.getEast();
        westedge = scenario.getWest();
        northedge = scenario.getNorth();
        southedge = scenario.getSouth();
    }

    /**
     * Advance time.  Recalculate the flow.
     * @param time the new time
     */
    @Override
    public void timerAdvance(int time) {
        // as we are using a sine rule for swing - convert to an angle (in radians)
        double radians = Math.toRadians(((double) time % period) / period * 360);
        swingNow = (int) (Math.sin(radians) * swing);
        // now dela with shifts
        int shiftval;
        double delta = randomshifts ? Math.random() * shiftperiod : time % shiftperiod;
        double quarterPeriod = shiftperiod / 4;
        if (delta < quarterPeriod) {
            shiftval = 0;
        } else if (delta < quarterPeriod * 2) {
            shiftval = -shift;
        } else if (delta < quarterPeriod * 3) {
            shiftval = 0;
        } else {
            shiftval = shift;
        }
        if (randomshifts) {
            // only apply the random shift in 2% of cases - otherwise leave alone
            if (Math.random() <= 0.02) {
                shiftNow = shiftval;
            }
        } else {
            shiftNow = shiftval; // apply the shift
        }
    }

    /**
     * Draw the Flow arrows on the display canvas.
     * 
     * @param g2D the 2D graphics object
     * @param pixelsPerMetre the scale factor
     */
    @Override
    public void draw(Graphics2D g2D, double pixelsPerMetre) {
        if (showFlow) { // draw the flow arrows
            //
            double x = westedge + showFlowInterval;
            while (x < eastedge) {
                double y = southedge + showFlowInterval;
                while (y < northedge) {
                    displayWindGraphic(g2D, pixelsPerMetre, x, y);
                    y += showFlowInterval;
                }
                x += showFlowInterval;
            }
        }
    }

    private void displayWindGraphic(Graphics2D g2D, double pixelsPerMetre, double x, double y) {
        GeneralPath p = new GeneralPath();
        p.moveTo(0, 15);
        p.lineTo(0, -15);
        p.moveTo(4, 7);
        p.lineTo(0, 15);
        p.lineTo(-4, 7);
        //
        AffineTransform xform = g2D.getTransform();
        g2D.translate(x, y);
        g2D.scale(1 / pixelsPerMetre, -1 / pixelsPerMetre);
        Flow flow = getFlow(new Location(x, y));
        g2D.rotate(flow.getDirection().getRadians());
        g2D.setColor(showFlowColour);
        g2D.setStroke(new BasicStroke(1));
        g2D.draw(p);
        //
        g2D.setFont(new Font("Sans Serif", Font.PLAIN, 10));
        NumberFormat nf = NumberFormat.getInstance();
        nf.setMaximumFractionDigits(1);
        nf.setMinimumFractionDigits(1);
        String windspeedText = nf.format(flow.getSpeedKnots());
        if (flow.getAngle() >= 0) {
            g2D.translate(-2, 4);
            g2D.rotate(-Math.PI / 2);
        } else {
            g2D.translate(+2, -15);
            g2D.rotate(Math.PI / 2);
        }
        g2D.drawString(windspeedText, 0, 0);
        g2D.setTransform(xform);
    }

    /**
     * Get the Flow at the current time, at the requested location.
     * 
     * @param pos location 
     * @return the flow
     */
    @Override
    public Flow getFlow(Location pos) {
        Flow f = getFlowWithoutSwing(pos);
        if (swinging) {
            f = new Flow(f);
            f.getDirection().add(swingNow);
        }
        if (shifting) {
            f = new Flow(f);
            f.getDirection().add(shiftNow);
        }
        return f;
    }

    private Flow getFlowWithoutSwing(Location pos) {
        // the flow is modelled over 9 sectors
        // first we can sort out the corner 4 sectors
        if (pos.x <= northwest.x && pos.y >= northwest.y) {
            return northwestFlow;
        }
        if (pos.x >= northeast.x && pos.y >= northeast.y) {
            return northeastFlow;
        }
        if (pos.x <= southwest.x && pos.y <= southwest.y) {
            return southwestFlow;
        }
        if (pos.x >= southeast.x && pos.y <= southeast.y) {
            return southeastFlow;
        }
        // calculate the locations and flows at the edges of the centre sector
        Location west = new Location(getX(northwest, southwest, pos.y), pos.y);
        Flow westFlow = flowAtY(northwest, northwestFlow, southwest, southwestFlow, pos.y);
        Location east = new Location(getX(northeast, southeast, pos.y), pos.y);
        Flow eastFlow = flowAtY(northeast, northeastFlow, southeast, southeastFlow, pos.y);
        Location north = new Location(pos.x, getY(northeast, northwest, pos.x));
        Flow northFlow = flowAtX(northeast, northeastFlow, northwest, northwestFlow, pos.x);
        Location south = new Location(pos.x, getY(southeast, southwest, pos.x));
        Flow southFlow = flowAtX(southeast, southeastFlow, southwest, southwestFlow, pos.x);
        // now look for the 4 centre out sectors
        if (pos.y > north.y) {
            return northFlow;
        }
        if (pos.y < south.y) {
            return southFlow;
        }
        if (pos.x < west.x) {
            return westFlow;
        }
        if (pos.x > east.x) {
            return eastFlow;
        }
        // must be the cental sector
        return flowAtY(north, northFlow, south, southFlow, pos.y);
    }

    private double getX(Location to, Location from, double y) {
        return from.x + (to.x - from.x) * (y - from.y) / (to.y - from.y);
    }

    private double getY(Location to, Location from, double x) {
        return from.y + (to.y - from.y) * (x - from.x) / (to.x - from.x);
    }

    private Flow flowAtX(Location to, Flow toFlow, Location from, Flow fromFlow, double x) {
        double ratio = (x - from.x) / (to.x - from.x);
        return new Flow((int) (fromFlow.getAngle() + ratio * (toFlow.getAngle() - fromFlow.getAngle())),
                fromFlow.getSpeedKnots() + ratio * (toFlow.getSpeedKnots() - fromFlow.getSpeedKnots()));
    }

    private Flow flowAtY(Location to, Flow toFlow, Location from, Flow fromFlow, double y) {
        double ratio = (y - from.y) / (to.y - from.y);
        return new Flow((int) (fromFlow.getAngle() + ratio * (toFlow.getAngle() - fromFlow.getAngle())),
                fromFlow.getSpeedKnots() + ratio * (toFlow.getSpeedKnots() - fromFlow.getSpeedKnots()));
    }

    /**
     * Get the Average flow across the total area.
     * 
     * @return the average flow
     */
    @Override
    public Flow getMeanFlow() {
        return new Flow((northeastFlow.getAngle() + northwestFlow.getAngle()
                + southeastFlow.getAngle() + southwestFlow.getAngle()) / 4,
                (northeastFlow.getSpeedKnots() + northwestFlow.getSpeedKnots()
                + southeastFlow.getSpeedKnots() + southwestFlow.getSpeedKnots()) / 4);
    }

    @Override
    protected Status setParameter(KeyPair kp) {
        try {
            switch (kp.key) {
                case "northeastposition":
                    northeast = parseLocation(kp.value);
                    return OK;
                case "northeastfrom":
                    northeastFlow.setAngle(Integer.parseInt(kp.value));
                    return OK;
                case "northeastspeed":
                    northeastFlow.setSpeedKnots(Double.parseDouble(kp.value));
                    return OK;
                case "northwestposition":
                    northwest = parseLocation(kp.value);
                    return OK;
                case "northwestfrom":
                    northwestFlow.setAngle(Integer.parseInt(kp.value));
                    return OK;
                case "northwestspeed":
                    northwestFlow.setSpeedKnots(Double.parseDouble(kp.value));
                    return OK;
                case "southeastposition":
                    southeast = parseLocation(kp.value);
                    return OK;
                case "southeastfrom":
                    southeastFlow.setAngle(Integer.parseInt(kp.value));
                    return OK;
                case "southeastspeed":
                    southeastFlow.setSpeedKnots(Double.parseDouble(kp.value));
                    return OK;
                case "southwestposition":
                    southwest = parseLocation(kp.value);
                    return OK;
                case "southwestfrom":
                    southwestFlow.setAngle(Integer.parseInt(kp.value));
                    return OK;
                case "southwestspeed":
                    southwestFlow.setSpeedKnots(Double.parseDouble(kp.value));
                    return OK;
                case "showflow":
                    showFlow = parseYesNo(kp.value);
                    return OK;
                case "showflowinterval":
                    showFlowInterval = Double.parseDouble(kp.value);
                    return OK;
                case "showflowcolour":
                    showFlowColour = parseColour(kp.value);
                    return OK;
                case "swingangle":
                    swing = Integer.parseInt(kp.value);
                    swinging = period > 0;
                    return OK;
                case "swingperiod":
                    period = Integer.parseInt(kp.value);
                    swinging = period > 0;
                    return OK;
                case "shiftangle":
                    shift = Integer.parseInt(kp.value);
                    shifting = shiftperiod > 0 || randomshifts;
                    return OK;
                case "shiftperiod":
                    shiftperiod = Integer.parseInt(kp.value);
                    shifting = shiftperiod > 0 || randomshifts;
                    return OK;
                case "randomshifts":
                    randomshifts = parseYesNo(kp.value);
                    shifting = shiftperiod > 0 || randomshifts;
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
                case "northeastposition":
                    parseLocation(kp.value);
                    return OK;
                case "northeastfrom":
                    Integer.parseInt(kp.value);
                    return OK;
                case "northeastspeed":
                    Double.parseDouble(kp.value);
                    return OK;
                case "northwestposition":
                    parseLocation(kp.value);
                    return OK;
                case "northwestfrom":
                    Integer.parseInt(kp.value);
                    return OK;
                case "northwestspeed":
                    Double.parseDouble(kp.value);
                    return OK;
                case "southeastposition":
                    parseLocation(kp.value);
                    return OK;
                case "southeastfrom":
                    Integer.parseInt(kp.value);
                    return OK;
                case "southeastspeed":
                    Double.parseDouble(kp.value);
                    return OK;
                case "southwestposition":
                    parseLocation(kp.value);
                    return OK;
                case "southwestfrom":
                    Integer.parseInt(kp.value);
                    return OK;
                case "southwestspeed":
                    Double.parseDouble(kp.value);
                    return OK;
                case "showflow":
                    parseYesNo(kp.value);
                    return OK;
                case "showflowinterval":
                    Double.parseDouble(kp.value);
                    return OK;
                case "showflowcolour":
                    parseColour(kp.value);
                    return OK;
                case "swingangle":
                    Integer.parseInt(kp.value);
                    return OK;
                case "swingperiod":
                    Integer.parseInt(kp.value);
                    return OK;
                case "shiftangle":
                    Integer.parseInt(kp.value);
                    return OK;
                case "shiftperiod":
                    Integer.parseInt(kp.value);
                    return OK;
                case "randomshifts":
                    parseYesNo(kp.value);
                    return OK;
                default:
                    return BADKEY;
            }
        } catch (NumberFormatException numberFormatException) {
            return BADVALUE;
        }
    }
}
