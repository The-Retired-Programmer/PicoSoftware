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
 * The Direction Class.
 *
 * @author Richard Linsdale (richard.linsdale at blueyonder.co.uk)
 */
public class Direction {

    /**
     * The direction (radians)
     */
    public double direction = 0;

    /**
     * Constructor.
     */
    public Direction() {
        direction = 0;
    }

    /**
     * Constructor.
     *
     * @param d direction (to be cloned)
     */
    public Direction(Direction d) {
        direction = d.getRadians();
    }

    /**
     * Constructor - direction is calculated from existing direction and further
     * turn (in degrees).
     *
     * @param d the original direction
     * @param degrees the additional turn (degrees)
     */
    public Direction(Direction d, int degrees) {
        direction = d.getRadians();
        add(degrees);
    }

    /**
     * Constructor.
     *
     * @param degrees the direction (degrees)
     */
    public Direction(int degrees) {
        set(degrees);
    }

    /**
     * Constructor - direction is calculated from existing direction and further
     * turn (in radians).
     *
     * @param d the original direction
     * @param radians the additional turn (radians)
     */
    public Direction(Direction d, double radians) {
        direction = d.getRadians();
        add(radians);
    }

    /**
     * Constructor.
     *
     * @param radians the direction (radians)
     */
    public Direction(double radians) {
        set(radians);
    }

    /**
     * Constructor - create a direction from one point to a second.
     *
     * @param from the first point
     * @param to the second point
     */
    public Direction(Location from, Location to) {
        direction = Math.atan2(to.x - from.x, to.y - from.y);
    }

    /**
     * Set the direction - the resulting direction is normalised (ie in range
     * -180 to 180 degrees)
     *
     * @param d the direction used to set the value.
     */
    public final void set(Direction d) {
        direction = d.direction;
        normaliseDirection();
    }

    /**
     * Set the direction - the resulting direction is normalised (ie in range
     * -180 to 180 degrees)
     *
     * @param degrees the direction (degrees)
     */
    public final void set(int degrees) {
        direction = Math.toRadians(degrees);
        normaliseDirection();
    }

    /**
     * Get the direction in degrees.
     *
     * @return the direction (degrees)
     */
    public final int getDegrees() {
        return (int) Math.round(Math.toDegrees(direction));
    }

    /**
     * Update the direction by adding a turn to the current direction - the
     * resulting direction is normalised (ie in range -180 to 180 degrees)
     *
     * @param degrees the additional angle (degrees)
     */
    public final void add(int degrees) {
        direction += Math.toRadians(degrees);
        normaliseDirection();
    }

    /**
     * Update the direction by subtracting a turn from the current direction -
     * the resulting direction is normalised (ie in range -180 to 180 degrees)
     *
     * @param degrees the subtracted angle (degrees)
     */
    public final void subtract(int degrees) {
        direction -= Math.toRadians(degrees);
        normaliseDirection();
    }

    /**
     * Get the angle difference between this direction and a given direction (in
     * degrees).
     *
     * @param degrees the given direction (degrees)
     * @return the angle difference (degrees)
     */
    public final int angleDiff(int degrees) {
        return (int) Math.toDegrees(normalise(Math.toRadians(degrees) - direction));
    }

    /**
     * Get the angle difference between this direction and a given direction.
     *
     * @param d the given direction
     * @return the angle difference (degrees)
     */
    public final int angleDiff(Direction d) {
        return angleDiff(d.getDegrees());
    }

    /**
     * Get the absolute angle difference between this direction and a given
     * direction (in degrees).
     *
     * @param degrees the given direction (degrees)
     * @return the absolute angle difference (degrees)
     */
    public final int absAngleDiff(int degrees) {
        return Math.abs(angleDiff(degrees));
    }

    /**
     * Get the absolute angle difference between this direction and a given
     * direction.
     *
     * @param d the given direction
     * @return the absolute angle difference (degrees)
     */
    public final int absAngleDiff(Direction d) {
        return Math.abs(angleDiff(d));
    }

    /**
     * Set the direction - the resulting direction is normalised (ie in range
     * -180 to 180 degrees)
     *
     * @param radians the direction (radians)
     */
    public final void set(double radians) {
        direction = radians;
        normaliseDirection();
    }

    /**
     * Get the direction in radians.
     *
     * @return the direction (radians)
     */
    public final double getRadians() {
        return direction;
    }

    /**
     * Update the direction by adding a turn to the current direction - the
     * resulting direction is normalised (ie in range -180 to 180 degrees)
     *
     * @param radians the additional angle (radians)
     */
    public final void add(double radians) {
        direction += radians;
        normaliseDirection();
    }

    /**
     * Update the direction by subtracting a turn from the current direction -
     * the resulting direction is normalised (ie in range -180 to 180 degrees)
     *
     * @param radians the additional angle (radians)
     */
    public final void subtract(double radians) {
        direction -= radians;
        normaliseDirection();
    }

    /**
     * Get the sine of this direction.
     *
     * @return the sine value
     */
    public final double sin() {
        return Math.sin(direction);
    }

    /**
     * Get the cosine of this direction.
     *
     * @return the cosine value
     */
    public final double cos() {
        return Math.cos(direction);
    }

    /**
     * Get the tangent of this direction.
     *
     * @return the tangent value
     */
    public final double tan() {
        return Math.tan(direction);
    }

    /**
     * Normalise a direction (in radians) - the normalised range is -pi to pi.
     *
     * @param direction the direction (radians)
     * @return the normalised direction (radians)
     */
    public static double normalise(double direction) {
        while (direction > Math.PI) {
            direction -= 2 * Math.PI;
        }
        while (direction < -Math.PI) {
            direction += 2 * Math.PI;
        }
        return direction;
    }

    private void normaliseDirection() {
        while (direction > Math.PI) {
            direction -= 2 * Math.PI;
        }
        while (direction < -Math.PI) {
            direction += 2 * Math.PI;
        }
    }
}
