package main

import (
	_"fmt"
	"time"
	
	"k8s.io/api/core/v1"
	"github.com/kubeedge/beehive/pkg/common/log"
	metav1 "k8s.io/apimachinery/pkg/apis/meta/v1"
	"github.com/kubeedge/kubeedge/cloud/pkg/apis/devices/v1alpha1"
	"github.com/kubeedge/kubeedge/cloud/pkg/deviceaccessor/demo"
	"github.com/kubeedge/kubeedge/cloud/pkg/deviceaccessor/manager"
)

func newDevice() *v1alpha1.Device {
	
	return &v1alpha1.Device{
		TypeMeta: metav1.TypeMeta {
			Kind: "Device",
			APIVersion: "devices.kubeedge.io/v1alpha1",
		},
		ObjectMeta: metav1.ObjectMeta {
			Name: "modbus-device0012",
			Namespace: "kubeedge",
		},
		Spec: v1alpha1.DeviceSpec {
			DeviceModelRef: &v1.LocalObjectReference{
				Name: "modbus-device-model",
			},
			Protocol:  v1alpha1.ProtocolConfig{
				Modbus: &v1alpha1.ProtocolConfigModbus{
					TCP: &v1alpha1.ProtocolConfigModbusTCP{
						IP: "172.21.73.226",
						Port: 532,
						SlaveID: "1",
					},
				},
			},
			NodeSelector: &v1.NodeSelector{
				NodeSelectorTerms: []v1.NodeSelectorTerm{
					{
						MatchExpressions: []v1.NodeSelectorRequirement{
							{
								Key: "",
								Operator: "In",
								Values: []string{"kgnode2"},
							},
						},
					},
				},
			},
		},
		Status: v1alpha1.DeviceStatus{
			Twins: []v1alpha1.Twin{
				{
					PropertyName: "Temperature",
					Desired: v1alpha1.TwinProperty{
						Value: "12",
						Metadata:map[string]string{
							"timestamp": "1550049403598",
							"type": "string",
						},
					},
					Reported:v1alpha1.TwinProperty{
						Value: "12",
						Metadata:map[string]string{
							"timestamp": "1550049403598",
							"type": "string",
						},
					},
				},
			},
		},
	}
}

func main(){
	log.LOGGER.Infof("New demo device controller")
	dc, err := demo.NewDemoController()	
	if err != nil {
		log.LOGGER.Warnf("New Demo controller error: %v", err)
		return
	}

	//Start the controller
	log.LOGGER.Infof("Start the controller")
	dc.Start()

loop:
	//Create the device
	device := newDevice()
	log.LOGGER.Infof("Create Device ....")
	err = dc.DeviceAccessor.CreateDevice(device, "kubeedge")
	if err != nil {
		log.LOGGER.Warnf("CreateDevice error: %v", err)
		return
	}
	log.LOGGER.Infof("Create Device Successful")

	//Update the device
	updateDevice := &manager.DeviceUpdate{
		DeviceID: "modbus-device0012",
		Status: &manager.DeviceStatus{
			Status: v1alpha1.DeviceStatus{
				Twins: []v1alpha1.Twin{
					{
						PropertyName: "Temperature",
						Desired: v1alpha1.TwinProperty{
							Value: "23",
							Metadata:map[string]string{
								"timestamp": "1550049403598",
								"type": "string",
							},
						},
						Reported:v1alpha1.TwinProperty{
							Value: "12",
							Metadata:map[string]string{
								"timestamp": "1550049403598",
								"type": "string",
							},
						},
					},
				},
			},
		},	
	}

	log.LOGGER.Infof("Update Device .....")
	err = dc.DeviceAccessor.UpdateDeviceStatus(updateDevice, "kubeedge")
	if err != nil {
		log.LOGGER.Warnf("UpdateDeviceStatus error: %v", err)
		return
	}
	log.LOGGER.Infof("Update Device Successful")

	//Delete the device 
	log.LOGGER.Infof("Delete the device ....")
	err = dc.DeviceAccessor.DeleteDevice("modbus-device0012", "kubeedge", nil)
	if err != nil {
		log.LOGGER.Warnf("DeleteDevice error: %v", err)
		return
	}
	log.LOGGER.Infof("Delete the device Successful")

	time.Sleep(10*time.Second)

	goto loop
}
