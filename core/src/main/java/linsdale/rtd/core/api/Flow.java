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
 * A movement vector containing direction and speed
 *
 * @author Richard Linsdale (richard.linsdale at blueyonder.co.uk)
 */
public class Flow {
    // direction is compass direction in degrees (-179 to +180)
    // of the flow (from)
    // speed is flow speed in knots

    private Direction direction;
    private double speed = 0; // in knots
    private static final double KNOTSTOMETRESPERSECOND = (double) 1853 / 3600; // multiply knots to get m/s
    // source NASA - 1 knot = 1.853 km/hour

    public Flow() {
        direction = new Direction();
        speed = 0;
    }

    public Flow(int angle, double speed) {
        this.direction = new Direction(angle);
        this.speed = speed;
    }

    public Flow(Flow copy) {
        this.direction = new Direction(copy.getDirection());
        this.speed = copy.getSpeedKnots();
    }

    public static int normaliseDirection(int direction) {
        while (direction > 180) {
            direction -= 360;
        }
        while (direction <= -180) {
            direction += 360;
        }
        return direction;
    }

    public void setAngle(int angle) {
        this.direction.set(angle);
    }

    public int getAngle() {
        return direction.getDegrees();
    }

    public Direction getDirection() {
        return direction;
    }

    public void setDirection(Direction direction) {
        this.direction = direction;
    }

    public void setSpeedKnots(double speed) {
        this.speed = speed;
    }

    public double getSpeedKnots() {
        return speed;
    }

    public double getSpeedMetresPerSecond() {
        return speed * KNOTSTOMETRESPERSECOND;
    }

    public static double convertKnots2MetresPerSecond(double knots) {
        return knots * KNOTSTOMETRESPERSECOND;
    }
}
