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

import linsdale.rtd.code.DefFileModel;

/**
 * The Abstract Factory Class.
 * 
 * @author Richard Linsdale (richard.linsdale at blueyonder.co.uk)
 * @param <C> the class of the rtd entity created by the factory
 */
public interface RtdFactory<C extends Rtd> {

    /**
     * Create a new Instance of an rtd object
     * @param name the name 
     * @param dfm the definition file data model
     * @return the rtd object
     */
    public C newInstance(String name, DefFileModel dfm);
}
