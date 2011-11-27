package com.dynamo.cr.go.core.test;

import javax.vecmath.Point3f;

import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Status;

import com.dynamo.cr.go.core.ComponentTypeNode;
import com.dynamo.cr.properties.Property;
import com.dynamo.cr.sceneed.core.Activator;

public class TestComponentNode extends ComponentTypeNode {

    @Property
    private Point3f position = new Point3f();

    public Point3f getPosition() {
        return this.position;
    }

    public void setPosition(Point3f position) {
        if (!this.position.equals(position)) {
            this.position = position;
            notifyChange();
        }
    }

    public IStatus validatePosition() {
        if (this.position.x == 0.0f && this.position.y == 0.0f && this.position.z == 0.0f) {
            return new Status(IStatus.INFO, Activator.PLUGIN_ID, "Position not yet set");
        }
        return Status.OK_STATUS;
    }

    @Override
    protected IStatus doValidate() {
        return validateProperties(new String[] {"position"});
    }
}
