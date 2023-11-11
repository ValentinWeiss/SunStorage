import { useTexture } from "@react-three/drei";
import { useEffect, useRef } from "react";

export const Sun = ({ sunPosition }: { sunPosition: THREE.Vector3 }) => {
    const ref = useRef<THREE.Group>(null);

    useEffect(() => {
        ref?.current?.position?.copy(sunPosition);
    }, [sunPosition]);

    const texture = useTexture("sun.png");
    return (
        <group ref={ref}>
            <sprite>
                <spriteMaterial attach="material" map={texture} />
            </sprite>
            <pointLight
                color="#f48037"
                intensity={10}
                decay={2}
                distance={20}
                castShadow
                shadow-mapSize-height={512}
                shadow-mapSize-width={512}
            />
        </group>
    );
};
