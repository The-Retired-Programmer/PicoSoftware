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

import java.awt.Graphics2D;
import linsdale.nbpcg.annotations.RegisterLog;
import linsdale.nbpcg.supportlib.OutputReporter;
import linsdale.rtd.code.DefFileModel;

/**
 *
 * @author Richard Linsdale (richard.linsdale at blueyonder.co.uk)
 */
@RegisterLog("linsdale.rta")
abstract public class RTAObject extends RTABaseObject {

    protected OutputReporter reporter;
    protected DefFileModel dfm;

    public RTAObject(String name, OutputReporter reporter, DefFileModel dfm) {
        super(name);
        this.dfm = dfm;
        this.reporter = reporter;
    }

    public void finish() {
        dfm = null;
    }

    abstract public void timerAdvance(int time);

    abstract public void draw(Graphics2D g2D, double pixelsPerMetre);
}
