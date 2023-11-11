import { useEffect, useRef } from "react";
import { Direction, GetPositionFromDirection } from "../";
import * as THREE from "three";

export const SolarPanel = ({ direction }: { direction?: Direction }) => {
    const stickHeight = 1;
    const panelRef = useRef<THREE.Mesh>();
    const stickRef = useRef<THREE.Mesh>();

    useEffect(() => {
        // cant look below floor so dont move..
        if (direction == null || direction.elevationAngle < 0) return;

        const vec = GetPositionFromDirection(direction);

        // look "stickHeight" higher since direction is to 0,0,0
        vec.y += stickHeight;
        panelRef.current?.lookAt(vec);
    }, [direction]);

    return (
        <>
            <mesh
                ref={stickRef}
                position={new THREE.Vector3(0, stickHeight / 2, 0)}
                castShadow
                receiveShadow
            >
                <cylinderBufferGeometry
                    attach="geometry"
                    args={[0.05, 0.05, 1]}
                />
                <meshStandardMaterial
                    attach="material"
                    color="#FFFFFF"
                    metalness={1}
                />
            </mesh>
            <mesh
                ref={panelRef}
                position={new THREE.Vector3(0, stickHeight, 0)}
                castShadow
                receiveShadow
            >
                <boxGeometry attach="geometry" args={[1, 1, 0.1]} />
                <meshStandardMaterial
                    attach="material"
                    color="#FFFFFF"
                    metalness={1}
                />
            </mesh>
        </>
    );
};
