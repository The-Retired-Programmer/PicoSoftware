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
package linsdale.rtd.mark;

import linsdale.rtd.core.api.RTAObjectFactory;
import linsdale.rtd.code.DefFileModel;
import linsdale.nbpcg.supportlib.OutputReporter;
import org.openide.util.lookup.ServiceProvider;

/**
 *
 * @author Richard Linsdale (richard.linsdale at blueyonder.co.uk)
 */
@ServiceProvider(service = MarkFactory.class)
public class MarkFactory implements RTAObjectFactory<Mark> {

    @Override
    public Mark newInstance(String name, OutputReporter reporter, DefFileModel dfm) {
        return new Mark(name, reporter, dfm);
    }
}
