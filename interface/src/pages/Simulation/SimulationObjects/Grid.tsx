import { Plane, useTexture } from "@react-three/drei";
import { Camera, useFrame, useThree } from "@react-three/fiber";
import { useContext, useRef } from "react";
import * as THREE from "three";
import { GridType, ISimulationContext, SimulationContext } from "../";

const XZCylinder = ({ size }: { size: number }) => (
    <mesh position={new THREE.Vector3(0, -0.1 / 2, 0)} receiveShadow castShadow>
        <cylinderBufferGeometry
            attach="geometry"
            args={[size / 2, size / 2, 0.1]}
        />
        <meshStandardMaterial attach="material" color="#FFFFFF" metalness={1} />
    </mesh>
);

const XZPlane = ({ size }: { size: number }) => (
    <Plane
        args={[size, size, size, size]}
        rotation={[1.5 * Math.PI, 0, 0]}
        position={[0, 0, 0]}
    >
        <meshStandardMaterial attach="material" color="#f9c74f" wireframe />
    </Plane>
);

const XYPlane = ({ size }: { size: number }) => (
    <Plane
        args={[size, size, size, size]}
        rotation={[0, 0, 0]}
        position={[0, 0, 0]}
    >
        <meshStandardMaterial attach="material" color="pink" wireframe />
    </Plane>
);

const YZPlane = ({ size }: { size: number }) => (
    <Plane
        args={[size, size, size, size]}
        rotation={[0, Math.PI / 2, 0]}
        position={[0, 0, 0]}
    >
        <meshStandardMaterial attach="material" color="#80ffdb" wireframe />
    </Plane>
);

export function Grid() {
    const pngN = useTexture("fonts/n.png");
    const pngO = useTexture("fonts/o.png");
    const pngS = useTexture("fonts/s.png");
    const pngW = useTexture("fonts/w.png");

    const { gridType: type, gridSize: size } =
        useContext<ISimulationContext>(SimulationContext);

    const textRefXp = useRef<THREE.Group>(null);
    const textRefXn = useRef<THREE.Group>(null);
    const textRefZp = useRef<THREE.Group>(null);
    const textRefZn = useRef<THREE.Group>(null);

    const { camera }: { camera: Camera } = useThree();

    useFrame(() => {
        textRefXp?.current?.rotation.copy(camera.rotation);
        textRefXn?.current?.rotation.copy(camera.rotation);
        textRefZp?.current?.rotation.copy(camera.rotation);
        textRefZn?.current?.rotation.copy(camera.rotation);
    });

    return (
        <>
            <group
                ref={textRefXp}
                position={new THREE.Vector3(size / 2 + 1, 0, 0)}
                scale={new THREE.Vector3(1, 1, 1)}
            >
                <sprite>
                    <spriteMaterial attach="material" map={pngO} />
                </sprite>
            </group>
            <group
                ref={textRefXn}
                position={new THREE.Vector3(-size / 2 - 1, 0, 0)}
                scale={new THREE.Vector3(1, 1, 1)}
            >
                <sprite>
                    <spriteMaterial attach="material" map={pngW} />
                </sprite>
            </group>
            <group
                ref={textRefZp}
                position={new THREE.Vector3(0, 0, size / 2 + 1)}
                scale={new THREE.Vector3(1, 1, 1)}
            >
                <sprite>
                    <spriteMaterial attach="material" map={pngS} />
                </sprite>
            </group>
            <group
                ref={textRefZn}
                position={new THREE.Vector3(0, 0, -size / 2 - 1)}
                scale={new THREE.Vector3(1, 1, 1)}
            >
                <sprite>
                    <spriteMaterial attach="material" map={pngN} />
                </sprite>
            </group>
            <group>
                {type === GridType.Test && (
                    <>
                        <XZPlane size={size} />
                        <XYPlane size={size} />
                        <YZPlane size={size} />
                    </>
                )}
                {type === GridType.Simulation && <XZCylinder size={size} />}
            </group>
        </>
    );
}
