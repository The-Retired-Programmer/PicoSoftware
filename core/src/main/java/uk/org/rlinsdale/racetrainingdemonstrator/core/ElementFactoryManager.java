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
package uk.org.rlinsdale.racetrainingdemonstrator.core;

import java.util.HashMap;
import java.util.Map;
import java.util.logging.Level;
import uk.org.rlinsdale.nbpcglibrary.common.Log;
import uk.org.rlinsdale.racetrainingdemonstrator.core.api.DisplayableElement;
import org.openide.util.Lookup;
import uk.org.rlinsdale.racetrainingdemonstrator.core.api.ElementFactory;

/**
 * Registry of all Factory Classes.
 * 
 * @author Richard Linsdale (richard.linsdale at blueyonder.co.uk)
 */
public class ElementFactoryManager {

    private final static Map<String, Class<? extends ElementFactory<?>>> factoryRegister = new HashMap<>();

    /**
     * Constructor.
     * 
     * @param classname the classname of the target class created by the factory
     * @param factoryClass the factory class
     */
    public static void registerElementFactory(String classname, Class<? extends ElementFactory<?>> factoryClass) {
        factoryRegister.put(classname, factoryClass);
    }

    private static Class<? extends ElementFactory<?>> getElementFactoryClass(String name) {
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
    public static DisplayableElement get(String classname, String instancename, AllElements dfm) {
        Log.get("uk.org.rlinsdale.racetrainingdemonstrator").log(Level.FINE, "Creating a new instance {0} of class {1}", new Object[]{
            instancename, classname
        });
        Class<? extends ElementFactory<?>> clazz = getElementFactoryClass(classname);
        if (clazz == null) {
            return null;
        }
        ElementFactory<?> factory = Lookup.getDefault().lookup(clazz);
        if (factory == null) {
            return null;
        }
        return factory.get(instancename, dfm);
    }
}
