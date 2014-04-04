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
package linsdale.rtd.laser2;

import linsdale.rtd.core.api.RtdFactory;
import linsdale.rtd.code.DefFileModel;
import org.openide.util.lookup.ServiceProvider;

/**
 * The Laser2 factory.
 * 
 * @author Richard Linsdale (richard.linsdale at blueyonder.co.uk)
 */
@ServiceProvider(service = Laser2Factory.class)
public class Laser2Factory implements RtdFactory<Laser2> {

    @Override
    public Laser2 newInstance(String name, DefFileModel dfm) {
        return new Laser2(name, dfm);
    }
}
