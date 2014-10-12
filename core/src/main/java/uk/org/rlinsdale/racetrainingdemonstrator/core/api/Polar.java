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
package uk.org.rlinsdale.racetrainingdemonstrator.core.api;

/**
 * A Polar location is a relative position from a logical origin expressed as a
 * distance and angle.
 *
 * @author Richard Linsdale (richard.linsdale at blueyonder.co.uk)
 */
public class Polar {
    // locations are relative

    private double r = 0;
    private double theta = 0;

    /**
     * Constructor. Create polar coordinates of a location from the 0,0
     * position.
     *
     * @param pos the location
     */
    public Polar(Location pos) {
        location2Polar(pos, new Location());
    }

    /**
     * Constructor.
     *
     * @param p the polar (used to clone the new Polar)
     */
    public Polar(Polar p) {
        this.r = p.r;
        this.theta = p.theta;
    }

    /**
     * Constructor. Create polar coordinates of a location from a given origin.
     *
     * @param pos the location
     * @param origin the origin location
     */
    public Polar(Location pos, Location origin) {
        location2Polar(pos, origin);
    }

    /**
     * Constructor. Create polar given polar coordinates.
     *
     * @param r the distance
     * @param theta the angle (radians)
     */
    public Polar(double r, double theta) {
        this.r = r;
        this.theta = theta;
        normaliseR();
    }

    /**
     * Constructor. Create polar given polar coordinates.
     *
     * @param r the distance
     * @param theta the angle (degrees)
     */
    public Polar(double r, int theta) {
        this.r = r;
        this.theta = Direction.normalise(Math.toRadians(normaliseAngle(theta)));
        normaliseR();
    }

    /**
     * Constructor. Create polar given distance and direction.
     *
     * @param r the distance
     * @param d the direction
     */
    public Polar(double r, Direction d) {
        this.r = r;
        this.theta = d.getRadians();
        normaliseR();
    }

    /**
     * Add a polar, updating existing polar coordinates.
     *
     * @param p the polar to add
     * @return this instance (with updated coordinates)
     */
    public Polar add(Polar p) {
        double x = r * Math.sin(theta) + p.r * Math.sin(p.theta);
        double y = r * Math.cos(theta) + p.r * Math.cos(p.theta);
        //
        r = Math.sqrt(x * x + y * y);
        theta = Math.atan2(x, y);
        return this;
    }

    /**
     * Add a distance at right angles, updating existing polar coordinates.
     *
     * @param distance the distance to add
     * @return this instance (with updated coordinates)
     */
    public Polar adddistance(double distance) {
        double x = r * Math.sin(theta) + distance * Math.cos(theta);
        double y = r * Math.cos(theta) - distance * Math.sin(theta);
        //
        r = Math.sqrt(x * x + y * y);
        theta = Math.atan2(x, y);
        return this;
    }

    /**
     * Subtract a polar, updating existing polar coordinates.
     *
     * @param p the polar to subtract
     * @return this instance (with updated coordinates)
     */
    public Polar subtract(Polar p) {
        double x = r * Math.sin(theta) - p.r * Math.sin(p.theta);
        double y = r * Math.cos(theta) - p.r * Math.cos(p.theta);
        //
        r = Math.sqrt(x * x + y * y);
        theta = Math.atan2(x, y);
        return this;
    }

    /**
     * Subtract a distance at right angles, updating existing polar coordinates.
     *
     * @param distance the distance to subtract
     * @return this instance (with updated coordinates)
     */
    public Polar subdistance(double distance) {
        double x = r * Math.sin(theta) - distance * Math.cos(theta);
        double y = r * Math.cos(theta) + distance * Math.sin(theta);
        //
        r = Math.sqrt(x * x + y * y);
        theta = Math.atan2(x, y);
        return this;
    }

    private void location2Polar(Location pos, Location origin) {
        double deltax = (pos.x - origin.x);
        double deltay = (pos.y - origin.y);
        r = Math.sqrt(deltax * deltax + deltay * deltay);
        theta = Math.atan2(pos.x - origin.x, pos.y - origin.y);
    }

    /**
     * Convert Polar to a Location (relative to a given origin).
     *
     * @param origin the origin location
     * @return the calculated location
     */
    public Location polar2Location(Location origin) {
        Location pos = new Location(origin);
        pos.x += r * Math.sin(theta);
        pos.y += r * Math.cos(theta);
        return pos;
    }

    /**
     * Get the Direction.
     *
     * @return the direction
     */
    public Direction getDirection() {
        return new Direction(theta);

    }

    /**
     * Get the angle difference between this polar and a given direction.
     *
     * @param direction the direction
     * @return the angle difference (degrees)
     */
    public int angleDiff(Direction direction) {
        return angleDiff(direction.getDegrees());
    }

    /**
     * Get the angle difference between this polar and a given angle.
     *
     * @param angle the angle (degrees)
     * @return the angle difference (degrees)
     */
    public int angleDiff(int angle) {
        return normaliseAngle((int) (angle - Math.toDegrees(theta)));
    }

    /**
     * Get the angle difference between this polar and a given polar.
     *
     * @param p the polar
     * @return the angle difference (degrees)
     */
    public int angleDiff(Polar p) {
        return angleDiff(p.getAngle());
    }

    /**
     * Get the absolute angle difference between this polar and a given
     * direction.
     *
     * @param direction the direction
     * @return the absolute angle difference (degrees)
     */
    public int absAngleDiff(Direction direction) {
        return Math.abs(angleDiff(direction));
    }

    /**
     * Get the absolute angle difference between this polar and a given angle.
     *
     * @param angle the angle (degrees)
     * @return the absolute angle difference (degrees)
     */
    public int absAngleDiff(int angle) {
        return Math.abs(angleDiff(angle));
    }

    /**
     * Get the Polar angle.
     *
     * @return the polar angle (degrees)
     */
    public int getAngle() {
        return (int) Math.round(Math.toDegrees(theta));
    }

    /**
     * Get the Polar distance.
     *
     * @return the distance
     */
    public double getDistance() {
        return r;
    }

    /**
     * Get the distance of a angle difference between this polar and the given
     * angle.
     *
     * @param angle the angle (degrees)
     * @return the calculated distance
     */
    public double getDistance(int angle) {
        return r * Math.abs(Math.cos(Math.toRadians(angleDiff(angle))));
    }

    private int normaliseAngle(int angle) {
        while (angle > 180) {
            angle -= 360;
        }
        while (angle < -180) {
            angle += 360;
        }
        return angle;
    }

    private void normaliseR() {
        if (r < 0) {
            r = Math.abs(r);
            theta = Direction.normalise(theta + Math.PI);

        }
    }
}
