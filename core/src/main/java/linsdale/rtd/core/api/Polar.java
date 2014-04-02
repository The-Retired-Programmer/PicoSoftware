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
package linsdale.rtd.core.api;

/**
 * A location is a relative position from a logical origin
 *
 * @author Richard Linsdale (richard.linsdale at blueyonder.co.uk)
 */
public class Polar {
    // locations are relative

    private double r = 0;
    private double theta = 0;

    public Polar(Location pos) {
        location2Polar(pos, new Location());
    }

    public Polar(Polar p) {
        this.r = p.r;
        this.theta = p.theta;
    }

    public Polar(Location pos, Location origin) {
        location2Polar(pos, origin);
    }

    public Polar(double r, double theta) {
        this.r = r;
        this.theta = theta;
        normaliseR();
    }

    public Polar(double r, int theta) {
        this.r = r;
        this.theta = Direction.normalise(Math.toRadians(normaliseAngle(theta)));
        normaliseR();
    }

    public Polar(double r, Direction d) {
        this.r = r;
        this.theta = d.getRadians();
        normaliseR();
    }

    public Polar add(Polar p) {
        double x = r * Math.sin(theta) + p.r * Math.sin(p.theta);
        double y = r * Math.cos(theta) + p.r * Math.cos(p.theta);
        //
        r = Math.sqrt(x * x + y * y);
        theta = Math.atan2(x, y);
        return this;
    }

    public Polar adddistance(double distance) {
        double x = r * Math.sin(theta) + distance * Math.cos(theta);
        double y = r * Math.cos(theta) - distance * Math.sin(theta);
        //
        r = Math.sqrt(x * x + y * y);
        theta = Math.atan2(x, y);
        return this;
    }

    public Polar subtract(Polar p) {
        double x = r * Math.sin(theta) - p.r * Math.sin(p.theta);
        double y = r * Math.cos(theta) - p.r * Math.cos(p.theta);
        //
        r = Math.sqrt(x * x + y * y);
        theta = Math.atan2(x, y);
        return this;
    }

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

    public Location polar2Location(Location origin) {
        Location pos = new Location(origin);
        pos.x += r * Math.sin(theta);
        pos.y += r * Math.cos(theta);
        return pos;
    }

    public Direction getDirection() {
        return new Direction(theta);

    }

    public int angleDiff(Direction direction) {
        return angleDiff(direction.getDegrees());
    }

    public int angleDiff(int angle) {
        return normaliseAngle((int) (angle - Math.toDegrees(theta)));
    }

    public int angleDiff(Polar p) {
        return angleDiff(p.getAngle());
    }

    public int absAngleDiff(Direction direction) {
        return Math.abs(angleDiff(direction));
    }

    public int absAngleDiff(int angle) {
        return Math.abs(angleDiff(angle));
    }

    public int getAngle() {
        return (int) Math.round(Math.toDegrees(theta));
    }

    public double getDistance() {
        return r;
    }

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
