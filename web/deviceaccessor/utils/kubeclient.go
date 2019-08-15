package utils

import (
	"os"
	
	"github.com/kubeedge/beehive/pkg/common/log"
	"github.com/kubeedge/kubeedge/cloud/pkg/apis/devices/v1alpha1"	

	"k8s.io/client-go/kubernetes"
	"k8s.io/client-go/rest"
	"k8s.io/apimachinery/pkg/runtime"
	"k8s.io/client-go/tools/clientcmd"
	"k8s.io/apimachinery/pkg/apis/meta/v1"
	"k8s.io/apimachinery/pkg/runtime/serializer"
)

// KubeConfig from env or inside of a pod in a cluster
func KubeConfig() (conf *rest.Config, err error) {
	//When running a binary inside of a pod in a cluster.
	config, err := rest.InClusterConfig()
	if err != nil {
		//fallback to kubeconfig
		kubeconfig := "/root/.kube/config"
		if envvar := os.Getenv("KUBECONFIG"); len(envvar) > 0 {
			kubeconfig = envvar
		}
	
		config, err = clientcmd.BuildConfigFromFlags("", kubeconfig)
		if err != nil {
			log.LOGGER.Warnf("Get kube config failed with error: %s", err)
			return nil, err
		}
	}

	return config, err
}

// KubeClient from config
func KubeClient() (*kubernetes.Clientset, error) {
	kubeConfig, err := KubeConfig()
	if err != nil {
		log.LOGGER.Warnf("Get kube config failed with error: %s", err)
		return nil, err
	}
	return kubernetes.NewForConfig(kubeConfig)
}


// NewCRDClient is used to create a restClient for crd
func NewCRDClient(cfg *rest.Config) (*rest.RESTClient, error) {
	scheme := runtime.NewScheme()
	schemeBuilder := runtime.NewSchemeBuilder(addDeviceCrds)

	err := schemeBuilder.AddToScheme(scheme)
	if err != nil {
		return nil, err
	}

	config := *cfg
	config.APIPath = "/apis"
	config.GroupVersion = &v1alpha1.SchemeGroupVersion
	config.ContentType = runtime.ContentTypeJSON
	config.NegotiatedSerializer = serializer.DirectCodecFactory{CodecFactory: serializer.NewCodecFactory(scheme)}

	client, err := rest.RESTClientFor(&config)
	if err != nil {
		log.LOGGER.Errorf("Failed to create REST Client due to error %v", err)
		return nil, err
	}

	return client, nil
}

func addDeviceCrds(scheme *runtime.Scheme) error {
	// Add Device
	scheme.AddKnownTypes(v1alpha1.SchemeGroupVersion, &v1alpha1.Device{}, &v1alpha1.DeviceList{})
	v1.AddToGroupVersion(scheme, v1alpha1.SchemeGroupVersion)
	// Add DeviceModel
	scheme.AddKnownTypes(v1alpha1.SchemeGroupVersion, &v1alpha1.DeviceModel{}, &v1alpha1.DeviceModelList{})
	v1.AddToGroupVersion(scheme, v1alpha1.SchemeGroupVersion)

	return nil
}
