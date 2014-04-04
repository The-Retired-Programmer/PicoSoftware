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
package linsdale.rtd.code;

import java.util.HashMap;
import java.util.Map;
import java.util.logging.Level;
import linsdale.nbpcg.supportlib.Log;
import linsdale.rtd.core.api.Rtd;
import org.openide.util.Lookup;
import linsdale.rtd.core.api.RtdFactory;

/**
 * Registry of all Factory Classes.
 * 
 * @author Richard Linsdale (richard.linsdale at blueyonder.co.uk)
 */
public class FactoryManager {

    private final static Map<String, Class<? extends RtdFactory<?>>> factoryRegister = new HashMap<>();

    /**
     * Constructor.
     * 
     * @param classname the classname of the target class created by the factory
     * @param factoryClass the factory class
     */
    public static void registerFactory(String classname, Class<? extends RtdFactory<?>> factoryClass) {
        factoryRegister.put(classname, factoryClass);
    }

    private static Class<? extends RtdFactory<?>> getFactoryClass(String name) {
        return factoryRegister.get(name);
    }

    /**
     * Create a new Instance of the target class.
     * 
     * @param classname the classname of the target class
     * @param instancename the name to be given to the instance
     * @param dfm the definition file datamodel
     * @return the new instance of the target class
     */
    public static Rtd newInstance(String classname, String instancename, DefFileModel dfm) {
        Log.get("linsdale.rtd").log(Level.FINE, "Creating a new instance {0} of class {1}", new Object[]{
            instancename, classname
        });
        Class<? extends RtdFactory<?>> clazz = getFactoryClass(classname);
        if (clazz == null) {
            return null;
        }
        RtdFactory<?> factory = Lookup.getDefault().lookup(clazz);
        if (factory == null) {
            return null;
        }
        return factory.newInstance(instancename, dfm);
    }
}
