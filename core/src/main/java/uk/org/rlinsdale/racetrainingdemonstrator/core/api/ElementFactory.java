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

import uk.org.rlinsdale.racetrainingdemonstrator.core.AllElements;

/**
 * The Abstract Factory Class.
 * 
 * @author Richard Linsdale (richard.linsdale at blueyonder.co.uk)
 * @param <C> the class of the rtd entity created by the factory
 */
public interface ElementFactory<C extends DisplayableElement> {

    /**
     * get an element
     * @param name the name 
     * @param elements the allelements registry
     * @return the element
     */
    public C get(String name, AllElements elements);
}
